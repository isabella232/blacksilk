#!/usr/bin/python

### Matching images with presets
imagesAndPresets = [
        ( "ajanta.tif" , "testing/preset_bwmixer_average.bs" ), # Test, 8bit TIFF Rendering
        ( "DSC03024.tif" , "testing/preset_bwmixer_average.bs" ), # Test, 16bit TIFF Rendering
        ( "nikon_d4s_28.tif" , "testing/preset_bwmixer_average.bs" ), # Test, 16bit TIFF Rendering
        ( "lima_peru_1400x525_with_alpha.png" , "testing/preset_bwmixer_average.bs" ), # Test, RGBA, 8bit PNG
        ( "lima_peru_1400x525.jpg" , "testing/preset_bwmixer_average.bs" ), # Test, RGB, 8bit JPG
        ( "DSC03024.tif" , "testing/preset_bwmixer_dual.bs" ), # Test, Dual Mixer-Test
        ( "DSC03024.tif" , "testing/preset_curves_highcontrast.bs" ), # Test, Curves
        ( "DSC03024.tif" , "testing/preset_curves_lowkey.bs" ), # Test, Curves
        ( "DSC03024.tif" , "testing/preset_grain_max_blur.bs" ), # Test, Grain
        ( "DSC03024.tif" , "testing/preset_grain_no_blur.bs" ), # Test, Grain
        ( "DSC03024.tif" , "testing/preset_sharpen_coarse_nothreshold.bs" ), # Test, Sharpen
        ( "DSC03024.tif" , "testing/preset_sharpen_fine.bs" ), # Test, Sharpen
        ( "DSC03024.tif" , "testing/preset_splittone_orange_teal.bs" ), # Test, Splittone
        ( "DSC03024.tif" , "testing/preset_splittone_sepia.bs" ), # Test, Splittone
        ( "DSC03024.tif" , "testing/preset_vignette_black.bs" ), # Test, Vignette
        ( "DSC03024.tif" , "testing/preset_vignette_white.bs" ), # Test, Vignette
        ( "DSC03024.tif" , "effects/preset_effect_retro.bs" ),
        ( "ajanta.tif" , "effects/preset_effect_max_contrast.bs" ),
        ( "nikon_d4s_28.tif" , "effects/preset_effect_dramatic.bs" )
    ]


### Common Types/Import Modules
class Systems:
    Unknown = -1
    OSX = 0
    Linux = 1
    Windows = 2

import sys
import subprocess
import os
import shutil
from sys import platform as _platform

### Platform Detection
if _platform == "linux" or _platform == "linux2":
    currentSystem = Systems.Linux
elif _platform == "darwin":
    currentSystem = Systems.OSX
elif _platform == "win32" or _platform == "cygwin":
    currentSystem = Systems.Windows
else:
    print "Error: Failed to query current platform. Aborting..."
    sys.exit(-1)


### Helper Utilities
def GetExePath(sys):
    if sys == Systems.OSX:
        return "../../bin/release/"
    elif sys == Systems.Linux:
        return "../../bin/release/"
    elif sys == Systems.Windows:
        return "../../bin/release/"
    else:
        return ""

def GetExeName(sys):
    if sys == Systems.OSX:
        return "BlackSilk.app/Contents/MacOS/BlackSilk"
    elif sys == Systems.Linux:
        return "BlackSilk"
    elif sys == Systems.Windows:
        return "BlackSilk.exe"
    else:
        return ""

def GetFullImagePath(path):
    return "resources/images/" + path

def GetFullPresetPath(path):
    return "resources/presets/" + path

def RunTest(path,image,preset):
    preset_name = os.path.basename(preset)
    out_file_path = "results/" + "filtered_" + preset_name + "_" + image
    with open(os.devnull, "w") as f:
        command = [path, GetFullImagePath(image), "--preset", GetFullPresetPath(preset), "--output", out_file_path ]
        print " ".join( command )
        ret = subprocess.call( command, shell=False, stdout=f )
        if ( ret == 0 ):
            print "Success: Filtered image '" + image + "' using preset '" + preset + "'. Filtered image was written to '" + out_file_path + "'."
            return True
        else:
            print "Error: Failed to filter image '" + image + "' using preset '" + preset + "'."
            return False
    return True

def AutoClearResults():
    retpath = "results"
    if os.path.exists(retpath):
        shutil.rmtree(retpath)
    os.mkdir(retpath)

def GetFullExeName():
    name = GetExeName(currentSystem)
    path = GetExePath(currentSystem)
    fullname = path+name
    print fullname
    if len(path) == 0:
        print "Error: Failed to query executable path. Aborting..."
        sys.exit(-1)
    return fullname


def Main():
    AutoClearResults()

    # Now run test
    successCount = 0
    notFoundCount = 0
    fullname = GetFullExeName()
    print "Running " + str(len(imagesAndPresets)) + " test cases..."

    for imageName,preset in imagesAndPresets:
        if os.path.exists(GetFullImagePath(imageName)) and os.path.exists(GetFullPresetPath(preset)):
            if RunTest(fullname,imageName,preset):
                successCount += 1
        else:
            print "Failed to run test-case: Image path or preset path do not exist."
            print "* image-path:  " + GetFullImagePath(imageName) + ", Exists: " + str(os.path.exists(GetFullImagePath(imageName)))
            print "* preset-path: " + GetFullPresetPath(preset) + ", Exists: " + str(os.path.exists(GetFullPresetPath(preset)))
            notFoundCount += 1

    print "Finished testing: " + str(successCount) + " out of " + str(len(imagesAndPresets)) + " test cases were successful."

    if notFoundCount > 0:
        print "Multiple resources have not been found( count: " + str(notFoundCount) + " ). Please copy missing resources from GDrive/FD-Imaging/Testing/integration-test-suite/ to integration-test-suite/resources/."
        sys.exit(-1)

Main()
