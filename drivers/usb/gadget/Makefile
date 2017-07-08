#
# USB peripheral controller drivers
#
subdir-ccflags-$(CONFIG_USB_GADGET_DEBUG)	:= -DDEBUG
subdir-ccflags-$(CONFIG_USB_GADGET_VERBOSE)	+= -DVERBOSE_DEBUG
ccflags-y				+= -I$(srctree)/drivers/usb/gadget/udc

obj-$(CONFIG_USB_LIBCOMPOSITE)	+= libcomposite.o
libcomposite-y			:= usbstring.o config.o epautoconf.o
libcomposite-y			+= composite.o functions.o configfs.o u_f.o

obj-$(CONFIG_USB_GADGET)	+= udc/ legacy/
obj-y				+= debug.o

ifeq ($(CONFIG_USB_GADGET),y)
 ifeq ($(CONFIG_USB_G_ANDROID),y)
 obj-y += legacy_function/
 else
 obj-y += function/
 endif
endif

obj-$(CONFIG_USB_CI13XXX_MSM) += udc/ci13xxx_msm.o
