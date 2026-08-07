
import math

from PyQt5 import QtCore, QtGui
from PyQt5.QtCore import *
from PyQt5.QtGui import *

Qt = QtCore.Qt

import miyamoto.spritelib as SLib

ImageCache = SLib.ImageCache

Rotations = [0, 0, 0]
StoneRotation = 0

class SpriteImage_TripleT(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -24
        self.yOffset = -16

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('TripleBlock_standard', 'triple_block_standard.png')
        SLib.loadIfNotInImageCache('TripleBlock_chika', 'triple_block_chika.png')
        SLib.loadIfNotInImageCache('TripleBlock_yougan', 'triple_block_yougan.png')
        SLib.loadIfNotInImageCache('TripleBlock_yougan2', 'triple_block_yougan2.png')
          
    def dataChanged(self):
        animationStyle = self.parent.spritedata[5] >> 4 & 0xF
            
        if animationStyle == 1:
            self.image = ImageCache['TripleBlock_chika']

        elif animationStyle == 2:
            self.image = ImageCache['TripleBlock_yougan']

        elif animationStyle == 3:
            self.image = ImageCache['TripleBlock_yougan2']
    
        else:
            self.image = ImageCache['TripleBlock_standard']

        super().dataChanged()



ImageClasses = {
    "blox:tripbk": SpriteImage_TripleT,
}