
import math

from PyQt5 import QtCore, QtGui
from PyQt5.QtCore import *
from PyQt5.QtGui import *

Qt = QtCore.Qt

import miyamoto.spritelib as SLib

ImageCache = SLib.ImageCache

Rotations = [0, 0, 0]
StoneRotation = 0

class BloxSpriteImage_ActorBlockHatenaLong(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_oos(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_oob(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_flipblock(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_psb(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_HatenaMimic(SLib.SpriteImage_Static):  # 819
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

class BloxSpriteImage_ActorBlockShock(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
            ImageCache['ActorBlockShock'],
            (-8, -16),
        )

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('ActorBlockShock', 'block_red_pow.png')

class BloxSpriteImage_ActorBlockHatenaSwitch(SLib.SpriteImage_Static):  # 819
    def __init__(self, parent):
        super().__init__(
            parent,
            3.75,
        )

        self.xOffset = -8
        self.yOffset = -16

    @staticmethod
    def loadImages():
        SLib.loadIfNotInImageCache('ActorBlockHatenaSwitch_p', 'block_hat_p_switch.png')
        SLib.loadIfNotInImageCache('ActorBlockHatenaSwitch_q', 'block_hat_q_switch.png')
          
    def dataChanged(self):
        switchtype = self.parent.spritedata[11] & 0x1
            
        if switchtype == 1:
            self.image = ImageCache['ActorBlockHatenaSwitch_p']
        else:
            self.image = ImageCache['ActorBlockHatenaSwitch_q']

class BloxSpriteImage_TileGod(SLib.SpriteImage_StaticMultiple):  # 237, 673
    def __init__(self, parent):
        super().__init__(parent, 3.75)
        self.aux2 = [SLib.AuxiliaryRectOutline(parent, 0, 0)]
        self.aux = self.aux2
        self.checkType = 0

    def dataChanged(self):
        super().dataChanged()

        self.checkType = (self.parent.spritedata[3] & 0xF)
        if self.checkType > 2:
            self.checkType = 1

        type_ = self.parent.spritedata[7] & 0xFF
        type_ = type_ + ((self.parent.spritedata[6] & 0x3) << 8)

        self.alpha = 1 if (self.parent.spritedata[4] >> 4 & 0xF) != 0 else 0.5
        
        self.width = (self.parent.spritedata[8] & 0xF) * 16
        self.height = (self.parent.spritedata[9] & 0xF) * 16

        if not self.width:
            self.width = 16

        if not self.height:
            self.height = 16

        if type_ > 0x3FF:
            self.aux = self.aux2
            self.spritebox.shown = True
            self.image = None

            if [self.width, self.height] == [16, 16]:
                self.aux2[0].setSize(0, 0)
                return
        else:
            self.aux = []
            self.spritebox.shown = False

            tile = SLib.Tiles[type_]

            if tile.exists:
                self.image = tile.main

            else:
                self.image = SLib.Tiles[0x200 * 4].main

        self.aux2[0].setSize(self.width * 3.75, self.height * 3.75)

    def paint(self, painter):
        if self.image is None:
            return

        painter.save()

        painter.setOpacity(self.alpha)
        painter.setRenderHint(painter.SmoothPixmapTransform)

        for yTile in range(self.height // 16):
            for xTile in range(self.width // 16):
                if ( self.checkType == 0                                                                                     # Solid Fill
                     or self.checkType == 1 and (xTile % 2 == 0 and yTile % 2 == 0 or xTile % 2 != 0 and yTile % 2 != 0)     # Checkers
                     or self.checkType == 2 and (xTile % 2 != 0 and yTile % 2 == 0 or xTile % 2 == 0 and yTile % 2 != 0) ):  # Inverted Checkers
                    painter.drawPixmap(xTile * 60, yTile * 60, self.image)

        aux = self.aux2
        aux[0].paint(painter, None, None)

        painter.restore()

ImageClasses = {
    "blox:tripbk": BloxSpriteImage_ActorBlockHatenaLong,
    "blox:flip": BloxSpriteImage_flipblock,
    "blox:oos": BloxSpriteImage_oos,
    "blox:oob": BloxSpriteImage_oob,
    "blox:psb": BloxSpriteImage_psb,
    "blox:htnamimic": BloxSpriteImage_HatenaMimic,
    "blox:actor_block_shock": BloxSpriteImage_ActorBlockShock,
    "blox:actor_block_hatena_switch": BloxSpriteImage_ActorBlockHatenaSwitch,
    "blox:change_block_plus": BloxSpriteImage_TileGod
}