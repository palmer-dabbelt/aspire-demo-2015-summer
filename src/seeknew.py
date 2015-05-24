# You will need to have python 2.7 (3+ may work)
# and PyUSB 1.0
# and PIL 1.1.6 or better
# and numpy
# and scipy
# and Tkinter
# and ImageMagick
# and other stuff when I get this working


# You will probably have to run this as root unless you get your udev/mdev rules
# set up to allow the Seek device to be used by other than root.

# Many thanks to the folks at eevblog, especially (in no particular order)
#   miguelvp, marshallh, mikeselectricstuff, sgstair, Fry-kun and many others
#     for the inspiration to figure this out
# This is not a finished product and you can use it if you like. Don't be
# suprised if there are bugs as I am NOT a programmer..... ;>))
# There is also a lot of test code sprinkled about which probably doesn't work.

# not all these are needed, must trim as required when this application is done

import usb.core
import usb.util
import sys
#from PIL import Image, ImageTk
import Image, ImageTk
import numpy
from scipy.misc import toimage
import Image
from numpy import array
import Tkinter

class App(Tkinter.Tk):
    def __init__(self,parent):
        Tkinter.Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

# defs

    def usbinit(self):
# find our Seek Thermal device  289d:0010
	dev = usb.core.find(idVendor=0x289d, idProduct=0x0010)

# was it found?
	if dev is None:
    	    raise ValueError('Device not found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
	dev.set_configuration()

# get an endpoint instance
	cfg = dev.get_active_configuration()
	intf = cfg[(0,0)]

	ep = usb.util.find_descriptor(
	    intf,
    # match the first OUT endpoint
    	    custom_match = \
    	    lambda e: \
		usb.util.endpoint_direction(e.bEndpointAddress) == \
    		usb.util.ENDPOINT_OUT)

	assert ep is not None

	return dev
#

    def camerainit(self,dev):

# Deinit the device

	msg= '\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x3C, 0, 0, msg) == len(msg)
	assert dev.ctrl_transfer(0x41, 0x3C, 0, 0, msg) == len(msg)
	assert dev.ctrl_transfer(0x41, 0x3C, 0, 0, msg) == len(msg)

# Setup device

#msg = x01
	assert dev.ctrl_transfer(0x41, 0x54, 0, 0, 0x01)

#  Some day we will figure out what all this init stuff is and
#  what the returned values mean.

	msg = '\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x3C, 0, 0, msg) == len(msg)

	ret1 = dev.ctrl_transfer(0xC1, 0x4E, 0, 0, 4)
	ret2 = dev.ctrl_transfer(0xC1, 0x36, 0, 0, 12)

#print ret1
#print ret2

#

	msg = '\x20\x00\x30\x00\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x56, 0, 0, msg) == len(msg)

	ret3 = dev.ctrl_transfer(0xC1, 0x58, 0, 0, 0x40)
#print ret3

#

	msg = '\x20\x00\x50\x00\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x56, 0, 0, msg) == len(msg)

	ret4 = dev.ctrl_transfer(0xC1, 0x58, 0, 0, 0x40)
#print ret4

#

	msg = '\x0C\x00\x70\x00\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x56, 0, 0, msg) == len(msg)

	ret5 = dev.ctrl_transfer(0xC1, 0x58, 0, 0, 0x18)
#print ret5

#

	msg = '\x06\x00\x08\x00\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x56, 0, 0, msg) == len(msg)

	ret6 = dev.ctrl_transfer(0xC1, 0x58, 0, 0, 0x0C)
#print ret6

#

	msg = '\x08\x00'
	assert dev.ctrl_transfer(0x41, 0x3E, 0, 0, msg) == len(msg)

	ret7 = dev.ctrl_transfer(0xC1, 0x3D, 0, 0, 2)
#print ret7

#

	msg = '\x08\x00'
	assert dev.ctrl_transfer(0x41, 0x3E, 0, 0, msg) == len(msg)
	msg = '\x01\x00'
	assert dev.ctrl_transfer(0x41, 0x3C, 0, 0, msg) == len(msg)

	ret8 = dev.ctrl_transfer(0xC1, 0x3D, 0, 0, 2)
#print ret8


#####################################################################
# build a matrix of "patent pixils" that match the Seek Imager
# this may be useful later
    def dots(self):
        dotsF = numpy.zeros((156,208))
        dotsI = dotsF.astype('uint8')
        k = 10

        for i in range(0,155,1):
            for j in range(k,206,15):
#                print i,j
		    dotsI[i,j] = 255
		    k = k - 4
    	    if k < 0: k = k + 15

	return dotsI
# display it to see if it matches the Seek black dot hex pattern

#	zz = Image.fromstring("I", (208,156), dotsI, "raw", "I;8")
#	toimage(zz).show()
#        print dotsI
#####################################################################


    def printIMG(self,img):
	global Label1, Label2
#    	print "img min= ", img.min()
#    	print "img max= ", img.max()
	j = img.min()
	k = img.max()
        Label1.configure( text="Img min %d" % j)
        Label2.configure( text="Img max %d" % k)

    def printCAL(self,cal):
	global Label3, Label4
#    	print "cal min= ", cal.min()
#    	print "cal max= ", cal.max()
	j = cal.min()
	k = cal.max()
        Label3.configure( text="Cal min %d" % j)
        Label4.configure( text="Cal max %d" % k)



    def printSUM(self,add):

	global Label5, Label6
#        print "sum min =", add.min()
#        print "sum max =", add.max()
	j = add.min()
	k = add.max()
        Label5.configure( text="Sum min %d" % j)
        Label6.configure( text="Sum max %d" % k)


    def print3(self):
        print additionF.shape
        print additionF.dtype
        print additionF.max()
        print additionF.min()
        print additionF.mean()

    def read_frame(self,dev): # Send a read frame request

	msg = '\xC0\x7E\x00\x00'
	assert dev.ctrl_transfer(0x41, 0x53, 0, 0, msg) == len(msg)

# For some reason, we need to read the result in 4 blocks, setting the
# read length to 4*0x3F60 returns only 0x3F60 bytes. Why?? maybe it is a
# PyUSB thing, since it seems to work elsewhere.

	data  = dev.read(0x81, 0x3F60, 1000)
	data += dev.read(0x81, 0x3F60, 1000)
	data += dev.read(0x81, 0x3F60, 1000)
	data += dev.read(0x81, 0x3F60, 1000)

	return data


    def calibrate(self, pix, cal):
        for i in range(0,156,1):
            for j in range(0,205,1):
                if cal[i,j] < 10:
                    if j>0:
                        # Duplicate the left pixel
                        pix[i,j] = pix[i,j-1]
                else:
                    pix[i,j] -= cal[i,j]
        return pix

    def add_207(self,imgF):  # Add the data from the 207 row to each pixil

# or not depending on the testing some of the following may be commented out.
# there are a different # of black dots in each row so the divisor
# needs to change for each row according to what is in the dot_numbers.txt file.
# this may not be the best way to do this. The code below does not do this now.

	for i in range(0,156,1):
	    for j in range(0,205,1):
		if imgF[i,j] == 0:
#		if dotsI[i,j] == 255:
		    imgF[i,j] = 0 # this should be all the "patent pixils" as well as the dead ones
		else:
		    imgF[i,j] = imgF[i,j]+imgF[i,206]/14

	return imgF


    def initialize(self):

	global dev, label, Label1, Label2, Label3, Label4, Label5, Label6

	dev = self.usbinit()
	self.camerainit(dev)

	self.grid()

#        self.entry = Tkinter.Entry(self)
#        self.entry.grid(column=0,row=0,sticky='EW')

#        button = Tkinter.Button(self,text=u"Click me !")
#        button.grid(column=1,row=0)

	Label1 = Tkinter.Label(self,text="Label1")
	Label2 = Tkinter.Label(self,text="Label2")
	Label3 = Tkinter.Label(self,text="Label3")
	Label4 = Tkinter.Label(self,text="Label4")
	Label5 = Tkinter.Label(self,text="Label5")
	Label6 = Tkinter.Label(self,text="Label6")

	Label1.grid(row=2,column=1)
	Label2.grid(row=3,column=1)
	Label3.grid(row=2,column=2)
	Label4.grid(row=3,column=2)
	Label5.grid(row=2,column=3)
	Label6.grid(row=3,column=3)

        label = Tkinter.Label(self,text="your image here", compound="top")

	label.grid(column=0,row=1,columnspan=2,sticky='EW')
	self.grid_columnconfigure(0,weight=1)
	self.resizable(True,True)

	self.iteration=0

	global calImage

        self.calimage = self.get_cal_image(dev)
	self.UpdateImage(10)


    def UpdateImage(self, delay, event=None):
        # this is merely so the display changes even though the image doesn't
	global dev, status, calImage, ImageFinal, label
        self.iteration += 1

        self.image = self.get_image(dev)
	ImageFinal = self.image

        label.configure(image=ImageFinal, text="Frames captured %s" % self.iteration)
        # reschedule to run again in 10 ms
        self.after(delay, self.UpdateImage, 10)

    def get_cal_image(self,dev):
# Get the first cal image so calImage isn't null

	global status, calImage
	status = 0

#  Wait for the cal frame

	while status != 1:
#  1 is a Calibration frame

# Read a raw frame
	   ret9 = self.read_frame(dev)

	   status = ret9[20]

#  6 is a pre-calibration frame (whatever that means)
#  4, 9, 8, 7, 5, 10 other... who knows.
#  See http://www.eevblog.com/forum/testgear/yet-another-cheap-thermal-imager-incoming/msg545910/#msg545910
#  for examples.


#  Convert the raw 16 bit calibration data to a PIL Image

	#calimgI = Image.frombytes("F", (208,156), ret9, "raw", "F;16")
	calimgI = Image.fromstring("F", (208,156), ret9, "raw", "F;16")

#  Convert the PIL Image to an unsigned numpy float array

	im2arr = numpy.asarray(calimgI)
	im2arrF = im2arr.astype('float')
#	im2arrI16 = im2arr.astype('uint16')
	calImage = im2arrF

	return

    def get_image(self,dev):
        # this is where you get your image and convert it to
        # a Tk PhotoImage. For demonstration purposes I'll
        # just return a static image
	global calImage, status

	status = 0

#  Wait for the next image frame

	while status != 3:
#  3 is a Normal frame


# Read a raw frame
	   ret9 = self.read_frame(dev)

	   status = ret9[20]

# check for a new cal frame, if so update the cal image

	   if status == 1:

#  Convert the raw 16 bit calibration data to a PIL Image

		#calimgI = Image.frombytes("F", (208,156), ret9, "raw", "F;16")
		calimgI = Image.fromstring("F", (208,156), ret9, "raw", "F;16")

#  Convert the PIL Image to an unsigned numpy float array

		im2arr = numpy.asarray(calimgI)
		im2arrF = im2arr.astype('float')

#  Save the calibration image

		calImage = im2arrF

#  If this is normal image data
#  Convert the raw 16 bit thermal data to a PIL Image

	imgx = Image.fromstring("F", (208,156), ret9, "raw", "F;16")

#  Convert the PIL Image to an unsigned numpy float array

	im1arr = numpy.asarray(imgx)
	im1arrF = im1arr.astype('float')

# Add the row 207 correction

#	im1arrF_207 = self.add_207(im1arrF)

	self.printIMG(im1arrF)
	self.printCAL(calImage)

#  Subtract the most recent calibration image from the offset image data

	#additionI16 = (((im1arrF) + 800) - (calImage))
        additionI16 = self.calibrate(im1arrF, calImage)

	self.printSUM(additionI16)

#	imxx = Image.fromarray(numpy.uint8(cm.gray(img)*255))

	imxx = toimage(additionI16,cmin=-1200,cmax=200).resize((640, 480),Image.ANTIALIAS)
	image = ImageTk.PhotoImage(imxx)
        return image

if __name__ == "__main__":
    app=App(None)
    app.title('Kens test')
    app.mainloop()
