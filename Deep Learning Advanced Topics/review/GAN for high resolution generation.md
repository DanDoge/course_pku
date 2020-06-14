While generative adversarial networks(GAN) can generate realistic images, they fail to do so at a larger scale. Some of the reasons they fail are

 a) the gradient direction becomes more random in high dimension space due to less overlap of the training and generated distributions, 

b) high dimensionality also make the discriminator more easily to differentiate the training and generated images, and

c) due to memory constraints, large resolution leads to smaller mini-batches, which further compromises training stability.

One way to tackle this issue is to focus on gradient directions. Other metrics have been proposed, including least squares(Mao et al. 2016b) and Wasserstein distance(Arjovsky et al 2017), in addition to the original Jensen-Shannon divergence. These methods are useful but not enough to alleviate GAN's instability at large scale.

An orthogonal approach aims to change the way GAN is being trained. The key observation is that instead of training all layers from scratch, GAN can be trained step by step, whether it be training a hierarchy of GANs, or training one layer at once. Denton et al. 2015 inserted a hierarchy of GANs into the Laplacian pyramid, while Huang et al. 2016(SGAN) further introduced a pretrained encoder to match intermediate representations. StackGAN and Wang et al. 2017(high-resolution w/ CGAN) both used a two-stage generation scheme to enable high-resolution synthesis. ProGAN further trains GAN in smaller steps: layer by layer, without pre-configured layers. Several tricks are applied to stabilize training: smooth fade-in of new layers, explicitly concatenating standard deviation, and more normalizations, including equalized learning rate, pixelwise normalization. ProGAN achieved more realistic results even using smaller mini-batches and trained faster compared to ones trained from scratch.