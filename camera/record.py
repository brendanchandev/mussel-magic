import logging
import os
import picamera
import subprocess
import time

logging.basicConfig(filename='recordings.log', datefmt='%d/%m/%Y %I:%M:%S %p', level=logging.DEBUG, format='%(asctime)s | %(message)s')


def start_recording(total_expected_videos=18, frame_rate=30, video_duration=3600):
    """
    :param total_expected_videos: total number of videos to be recorded
    :param frame_rate: framerate of the videos
    :param video_duration: time duration of each video
    :return:
    """
    with picamera.PiCamera() as camera:
        for video_idx in range(total_expected_videos):
            try:
                start_time = time.time()
                input_filename = "mussels.h264"
                output_filename = "mussels-" + time.strftime("%Y%m%d-%H%M%S") + ".mp4"

                # rotate camera to 180 degrees, if required
                camera.rotation = 180
                camera.resolution = (854, 480)

                print("Recording {} started at {}, wait for {} minutes".format(video_idx + 1, time.strftime('%I:%M:%S %p'), video_duration/60))
                camera.start_recording(input_filename)
                camera.wait_recording(video_duration)
                camera.stop_recording()

                convert_to_mp4 = "ffmpeg -framerate {} -i {} -c copy {}".format(frame_rate, input_filename, output_filename)
                subprocess.call(convert_to_mp4, shell=True)
                os.remove(input_filename)

                logging.debug("{} - Successfully exported {} in {} seconds".format(video_idx + 1, output_filename, time.time() - start_time))
                print("{} - Successfully exported {} in {} seconds".format(video_idx + 1, output_filename, time.time() - start_time))

            except:
                logging.error("{} - Failed to export {}".format(video_idx + 1, output_filename))
                print("{} - Failed to export {}".format(video_idx + 1, output_filename))


if __name__ == '__main__':
    start_recording()
