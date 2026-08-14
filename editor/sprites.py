
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

class SpriteImage_oos(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -8
        self.yOffset = -16

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('OnOffSwitch_standard', 'on_off_switch_standard.png')
        SLib.loadIfNotInImageCache('OnOffSwitch_chika', 'on_off_switch_chika.png')
        SLib.loadIfNotInImageCache('OnOffSwitch_yougan', 'on_off_switch_yougan.png')
        SLib.loadIfNotInImageCache('OnOffSwitch_yougan2', 'on_off_switch_yougan2.png')
          
    def dataChanged(self):
        animationStyle = self.parent.spritedata[5] >> 4 & 0xF
            
        if animationStyle == 1:
            self.image = ImageCache['OnOffSwitch_chika']

        elif animationStyle == 2:
            self.image = ImageCache['OnOffSwitch_yougan']

        elif animationStyle == 3:
            self.image = ImageCache['OnOffSwitch_yougan2']
    
        else:
            self.image = ImageCache['OnOffSwitch_standard']

        super().dataChanged()

class SpriteImage_oob(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -8
        self.yOffset = -8

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('OnOffBlockRed_standard', 'on_off_block_red_standard.png')
        SLib.loadIfNotInImageCache('OnOffBlockRed_chika', 'on_off_block_red_chika.png')
        SLib.loadIfNotInImageCache('OnOffBlockRed_yougan', 'on_off_block_red_yougan.png')
        SLib.loadIfNotInImageCache('OnOffBlockRed_yougan2', 'on_off_block_red_yougan2.png')
        SLib.loadIfNotInImageCache('OnOffBlockBlue', 'on_off_block_blue.png')
          
    def dataChanged(self):
        animationStyle = self.parent.spritedata[5] >> 4 & 0xF
        isBlue = self.parent.spritedata[2] >> 4 & 0xF
            
        if isBlue == 1:
            self.image = ImageCache['OnOffBlockBlue']
        else:
            if animationStyle == 1:
                self.image = ImageCache['OnOffBlockRed_chika']
            elif animationStyle == 2:
                self.image = ImageCache['OnOffBlockRed_yougan']
            elif animationStyle == 3:
                self.image = ImageCache['OnOffBlockRed_yougan2']
            else:
                self.image = ImageCache['OnOffBlockRed_standard']

        super().dataChanged()

class SpriteImage_flipblock(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
            ImageCache['flipblock'],
            (0, -8),
        )

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('flipblock', 'flipblock.png')

class SpriteImage_psb(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -8
        self.yOffset = -8

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('PBlockRed_standard', 'p_block_red_standard.png')
        SLib.loadIfNotInImageCache('PBlockRed_chika', 'p_block_red_chika.png')
        SLib.loadIfNotInImageCache('PBlockRed_yougan', 'p_block_red_yougan.png')
        SLib.loadIfNotInImageCache('PBlockRed_yougan2', 'p_block_red_yougan2.png')
        SLib.loadIfNotInImageCache('PBlockBlue', 'p_block_blue.png')
          
    def dataChanged(self):
        animationStyle = self.parent.spritedata[5] >> 4 & 0xF
        isBlue = self.parent.spritedata[2] >> 4 & 0xF
            
        if isBlue == 1:
            self.image = ImageCache['PBlockBlue']
        else:
            if animationStyle == 1:
                self.image = ImageCache['PBlockRed_chika']
            elif animationStyle == 2:
                self.image = ImageCache['PBlockRed_yougan']
            elif animationStyle == 3:
                self.image = ImageCache['PBlockRed_yougan2']
            else:
                self.image = ImageCache['PBlockRed_standard']

class SpriteImage_HatenaMimic(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -8
        self.yOffset = -16

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('qbl16', 'qbl16.png')
        for j in range(16):
            SLib.loadIfNotInImageCache('qbl{0}'.format(j),
                                        'qbl{0}.png'.format(j))
          
    def dataChanged(self):
        qblCts = self.parent.spritedata[4] >> 4
            
        if qblCts > 16:
            self.image = ImageCache['qbl0']
        else:
            self.image = ImageCache['qbl{0}'.format(qblCts)]


ImageClasses = {
    "blox:tripbk": SpriteImage_TripleT,
    "blox:flip": SpriteImage_flipblock,
    "blox:oos": SpriteImage_oos,
    "blox:oob": SpriteImage_oob,
    "blox:psb": SpriteImage_psb,
    "blox:htnamimic": SpriteImage_HatenaMimic,
}