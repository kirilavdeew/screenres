/*
*  osx.cc
*  David Mark Clements 2015
*  @davidmarkclem
*
*  Based on 'setgetscreenres'
*   juanfc 2009-04-13
*   Based on newscreen
*      Created by Jeffrey Osterman on 10/30/07.
*      Copyright 2007 Jeffrey Osterman. All rights reserved.
*      PROVIDED AS IS AND WITH NO WARRANTIES WHATSOEVER
*      http://forums.macosxhints.com/showthread.php?t=59575
*
*/

#ifdef __APPLE__

#include <iostream>
using namespace std;
#include <ApplicationServices/ApplicationServices.h>

#include "headers.h"

typedef struct {
    uint32_t modeNumber;
    uint32_t flags;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint8_t unknown[170];
    uint16_t freq;
    uint8_t more_unknown[16];
    float density;
} CGSDisplayMode;

extern "C" void CGSGetNumberOfDisplayModes(CGDirectDisplayID display, int *nModes);
extern "C" void CGSGetDisplayModeDescriptionOfLength(CGDirectDisplayID display, int idx, CGSDisplayMode *mode, int length);
extern "C" void CGSConfigureDisplayMode(CGDisplayConfigRef config, CGDirectDisplayID display, int modeNum);

bool MyDisplaySwitchToMode (CGDirectDisplayID display, int mode);

Resolution fetchRes() {
    CGRect screenFrame = CGDisplayBounds(kCGDirectMainDisplay);
    CGSize screenSize  = screenFrame.size;
    Resolution res = {
      .width = (int)screenSize.width,
      .height = (int)screenSize.height
    };
    return res;
}

int changeRes (int h, int v) {
  //CGDisplayModeRef switchMode = NULL;   // mode to switch to
  uint32_t modeNumber = 0;
  CGDirectDisplayID mainDisplay;  // ID of main display
  CFDictionaryRef CGDisplayCurrentMode(CGDirectDisplayID display);

  if (!h || !v) {
    return ERR_MISSING_H_OR_V;
  }

  mainDisplay = CGMainDisplayID();
  //switchMode = CGDisplayBestModeForParameters(mainDisplay, 32, h, v, NULL);
  // CFArrayRef modes = CGDisplayCopyAllDisplayModes(mainDisplay, NULL);
  // int count = CFArrayGetCount(modes);
  // for (int i = 0;  i < count;  i++) {
  //   CGDisplayModeRef mode = (CGDisplayModeRef) CFArrayGetValueAtIndex(modes, i);
  //   if(h == (int)CGDisplayModeGetWidth(mode) && v == (int)CGDisplayModeGetHeight(mode)
  // && CGDisplayModeIsUsableForDesktopGUI(mode)) {
  //     switchMode = mode;
  //     uint32_t flags=CGDisplayModeGetIOFlags(mode);
	//     //NSLog(CFSTR(snprintf("flags: %d"), flags);
	//     //NSLog("w, h: %ld, %ld", h, v);
  //     cout << (int)CGDisplayModeGetWidth(mode)<< ", " << (int)CGDisplayModeGetHeight(mode) << endl;
  //   }
  // }
  // if(switchMode) {
    //cout << "aaaa" << endl;
    int numberOfDisplayModes;
    CGSGetNumberOfDisplayModes(mainDisplay, &numberOfDisplayModes);
    //cout << numberOfDisplayModes << endl;
    float savedDensity = -1.0;
    for (int i = 0; i < numberOfDisplayModes; i++) {
        CGSDisplayMode mode2;
        CGSGetDisplayModeDescriptionOfLength(mainDisplay, i, &mode2, sizeof(mode2));
        //cout << (int)mode2.width <<" x "<< (int)mode2.height << endl;
        if( h == (int)mode2.width && v == (int)mode2.height) {
          //cout << "Find "<< mode2.width << ", " << mode2.height << " d = "<< mode2.density << endl;
          if(savedDensity<mode2.density) {
            savedDensity = mode2.density;
            modeNumber = mode2.modeNumber;
          }
        }
    }
    //exit(0);
//  }
  if (!MyDisplaySwitchToMode(mainDisplay, modeNumber)) {
    return ERR_UNABLE_TO_CHANGE_RESOLUTION;
  }

  return 0;
}

bool MyDisplaySwitchToMode (CGDirectDisplayID display, int modeNumber) {
  CGDisplayConfigRef config;
  if (CGBeginDisplayConfiguration(&config) == kCGErrorSuccess) {
    CGSConfigureDisplayMode(config, display, modeNumber);
    //CGConfigureDisplayWithDisplayMode(config, display, mode, NULL);
    CGCompleteDisplayConfiguration(config, kCGConfigureForSession );
    return true;
  }
  return false;
}

#endif
