import tensorflow as tf
import tensorflow_hub as hub
import matplotlib.pyplot as plt
import numpy as np
import sys

content_image_path = sys.argv[1]
style_image_path = sys.argv[2]
output_path = sys.argv[3]

# Load content and style images (see example in the attached colab).
content_image = plt.imread(content_image_path)
style_image = plt.imread(style_image_path)
# Convert to float32 numpy array, add batch dimension, and normalize to range [0, 1]. Example using numpy:
content_image = content_image.astype(np.float32)[np.newaxis, ...]# / 255.
content2 = content_image[:,:,:,:3]
style_image = style_image.astype(np.float32)[np.newaxis, ...]# / 255.
style2 = style_image[:,:,:,:3]
# Optionally resize the images. It is recommended that the style image is about
# 256 pixels (this size was used when training the style transfer network).
# The content image can be any size.
style2 = tf.image.resize(style2, (256, 256))

# Load image stylization module.
hub_module = hub.load('https://tfhub.dev/google/magenta/arbitrary-image-stylization-v1-256/2')

# Stylize image.
outputs = hub_module(tf.constant(content2), tf.constant(style2))
stylized_image = outputs[0]
# plt.imshow(stylized_image[0])
# plt.waitforbuttonpress()
plt.imsave(output_path,np.array(stylized_image[0]))
