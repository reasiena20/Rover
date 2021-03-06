import cv2
import time
import threading
from flask import Response, Flask
import depth_viewer
import numpy as np

# Image frame sent to the Flask object
global video_frame
video_frame = None

# Use locks for thread-safe viewing of frames in multiple browsers
global thread_lock 
thread_lock = threading.Lock()


# Create the Flask object for the application
app = Flask(__name__)


def captureFrames():
	global video_frame, thread_lock
	pipeline = depth_viewer.startPipeline()
	try:
		# Video capturing from OpenCV
		while True:
			frames = pipeline.wait_for_frames()
			depth_frame = frames.get_depth_frame()
			color_frame = frames.get_color_frame()
			if not depth_frame or not color_frame:
				continue

			# Convert images to numpy arrays
			depth_image = np.asanyarray(depth_frame.get_data())
			color_image = np.asanyarray(color_frame.get_data())

			# Apply colormap on depth image (image must be converted to 8-bit per pixel first)
			depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)

			# Stack both images horizontally
			images = np.hstack((color_image, depth_colormap))
			

			#Downscale image for faster streaming
			newWidth = int(images.shape[1] / 2)
			newHeight = int(images.shape[0] / 2)
			images = cv2.resize(images, (newWidth, newHeight), interpolation = cv2.INTER_AREA)			

			# Create a copy of the frame and store it in the global variable,
			# with thread safe access
			with thread_lock:
				video_frame = images.copy()
			cv2.imshow('name', images)
			key = cv2.waitKey(30) & 0xff
			if key == 27:
				break
		cv2.destroyAllWindows()
	finally:
		pipeline.stop()
        
def encodeFrame():
    global thread_lock
    while True:
        # Acquire thread_lock to access the global video_frame object
        with thread_lock:
            global video_frame
            if video_frame is None:
                continue
            return_key, encoded_image = cv2.imencode(".jpg", video_frame)
            if not return_key:
                continue

        # Output image as a byte array
        yield(b'--frame\r\n' b'Content-Type: image/jpeg\r\n\r\n' + 
            bytearray(encoded_image) + b'\r\n')

@app.route("/")
def streamFrames():
    return Response(encodeFrame(), mimetype = "multipart/x-mixed-replace; boundary=frame")

# check to see if this is the main thread of execution
if __name__ == '__main__':

    # Create a thread and attach the method that captures the image frames, to it
    process_thread = threading.Thread(target=captureFrames)
    process_thread.daemon = True

    # Start the thread
    process_thread.start()

    # start the Flask Web Application
    # While it can be run on any feasible IP, IP = 0.0.0.0 renders the web app on
    # the host machine's localhost and is discoverable by other machines on the same network 
    app.run("0.0.0.0", port="8000")
