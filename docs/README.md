# TensorFlow-Lite-Object-Detection-on-Android-and-Raspberry-Pi
## Introduction
This is an addition to EdjeElectronics' TensorFlow-Lite-Object-Detection-on-Android-and-Raspberry-Pi which allows us to control stepper motors to move a camera so the image remains centered.
## Install
Install everything EdjeElectronics installed (view repo: https://github.com/EdjeElectronics/TensorFlow-Lite-Object-Detection-on-Android-and-Raspberry-Pi)
After this, download this repo and place everything in the tflite1 (or equivalent) folder. You can choose to replace TFLite_Detection_webcam.py, it's not really relevant anymore.
Start the virtual environment.

```
source tflite1-env/bin/activate
```

And run the install_serial.sh script, this allows communication over the usb port. (This is only necessairy forthe first time.)

```
bash install_serial.sh
```

Now you can run TFLite_Detection_webcam_send.py. The threshold describes how high the score has to be to show the object.

```
python3 TFLite_detection_webcam_serial.py --modeldir=Sample_TFLite_model --threshold=0.6 
```


I made a little script to aid in this process in the future, this also starts the venv.

```
bash start_webcam_serial.sh
```

## Description
The project currently allows one object (the one with the highest score) from one class to be detected. This makes it a lot easier to follow only one object.
The script returns a string and sends this over the usb port. This can can be read by the Arduino. (scripts are currently available under expermental(yes, this is a typo))
