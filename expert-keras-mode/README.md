
### Started June 29th, 2022


# A location for files EdgeImpulse example expert mode Keras files (.txt) 

Use .txt format unless we can think of a better format.

Hopefully add a few comments, and name the file with the main type of model:

vision-fomo, vision-classify, sound, motion or something else relevant to what the model does.

examples

fomo-3-extra-layers.txt


Reminder to post the enitre Keras file, not just the relevant parts.


Best reference is probably here https://docs.edgeimpulse.com/docs/edge-impulse-studio/learning-blocks/object-detection/fomo-object-detection-for-constrained-devices#expert-mode-tips

The Issue: What to have a generic model starting point for any combination of students connected sensors

best sensor attempt so far- this is the default Keras Classifier

Which happens to be exactly the same as the deafult motion model

Any sensor model?

```
model.add(Dense(20, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dense(10, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dense(classes, activation='softmax', name='y_pred'))

```



The meat of the above files:

Motion-default
```
model.add(Dense(20, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dense(10, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dense(classes, activation='softmax', name='y_pred'))

```

Sound-deafult
```
model.add(Reshape((int(input_length / 13), 13), input_shape=(input_length, )))
model.add(Conv1D(8, kernel_size=3, activation='relu', padding='same'))
model.add(MaxPooling1D(pool_size=2, strides=2, padding='same'))
model.add(Dropout(0.25))
model.add(Conv1D(16, kernel_size=3, activation='relu', padding='same'))
model.add(MaxPooling1D(pool_size=2, strides=2, padding='same'))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(classes, activation='softmax', name='y_pred'))

```

Vision-classify-regression
```
model.add(Conv2D(32, kernel_size=3, activation='relu', kernel_constraint=tf.keras.constraints.MaxNorm(1), padding='same'))
model.add(MaxPooling2D(pool_size=2, strides=2, padding='same'))
model.add(Conv2D(16, kernel_size=3, activation='relu', kernel_constraint=tf.keras.constraints.MaxNorm(1), padding='same'))
model.add(MaxPooling2D(pool_size=2, strides=2, padding='same'))
model.add(Flatten())
model.add(Dropout(0.25))
model.add(Dense(classes, name='y_pred'))


```

Vision Classify-default
```
model.add(Conv2D(32, kernel_size=3, activation='relu', kernel_constraint=tf.keras.constraints.MaxNorm(1), padding='same'))
model.add(MaxPooling2D(pool_size=2, strides=2, padding='same'))
model.add(Conv2D(16, kernel_size=3, activation='relu', kernel_constraint=tf.keras.constraints.MaxNorm(1), padding='same'))
model.add(MaxPooling2D(pool_size=2, strides=2, padding='same'))
model.add(Flatten())
model.add(Dense(10, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dropout(0.1))
model.add(Dense(4, activation='relu', activity_regularizer=tf.keras.regularizers.l1(0.00001)))
model.add(Dense(classes, activation='softmax', name='y_pred'))


```



FOMO-default (Note: A fair bit more complex than the above mode3ls!)

```
    mobile_net_v2 = MobileNetV2(input_shape=input_shape, weights=weights, alpha=alpha, include_top=True)
    #! Default batch norm is configured for huge networks, let's speed it up
    for layer in mobile_net_v2.layers:
        if type(layer) == BatchNormalization:
            layer.momentum = 0.9
            
    #! Cut MobileNet where it hits 1/8th input resolution; i.e. (HW/8, HW/8, C)
    cut_point = mobile_net_v2.get_layer('block_6_expand_relu')
    
    #! Now attach a small additional head on the MobileNet
    model = Conv2D(filters=32, kernel_size=1, strides=1, activation='relu', name='head')(cut_point.output)
    logits = Conv2D(filters=num_classes, kernel_size=1, strides=1, activation=None, name='logits')(model)
    return Model(inputs=mobile_net_v2.input, outputs=logits)
```

The EON turner will deffinitely help determine the best architecture, however I would still prefer a great starting point.


![image](https://user-images.githubusercontent.com/5605614/177057111-ecaedddc-2f7c-49fd-a90f-581b403f673a.png)








not really related but just found out about custom DSP blocks  see tutorial herre https://docs.edgeimpulse.com/docs/edge-impulse-studio/organizations/hosting-custom-dsp-block
