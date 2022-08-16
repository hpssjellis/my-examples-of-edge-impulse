#!/bin/bash

unzip detection-tensorflow-savedmodel-model.zip
tensorflowjs_converter --input_format=tf_saved_model  ./saved_model/ ./for_tfjs_fomo/


