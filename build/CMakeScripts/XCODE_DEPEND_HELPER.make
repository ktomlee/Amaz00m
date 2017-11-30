# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.robot.Debug:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/robot:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/robot


PostBuild.warehouse_main.Debug:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/warehouse_main:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/warehouse_main


PostBuild.warehouse_ui.Debug:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/warehouse_ui:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Debug/warehouse_ui


PostBuild.robot.Release:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Release/robot:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Release/robot


PostBuild.warehouse_main.Release:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Release/warehouse_main:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Release/warehouse_main


PostBuild.warehouse_ui.Release:
/Users/tomlee/Documents/GitHub/Amaz00m/build/Release/warehouse_ui:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/Release/warehouse_ui


PostBuild.robot.MinSizeRel:
/Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/robot:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/robot


PostBuild.warehouse_main.MinSizeRel:
/Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/warehouse_main:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/warehouse_main


PostBuild.warehouse_ui.MinSizeRel:
/Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/warehouse_ui:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/MinSizeRel/warehouse_ui


PostBuild.robot.RelWithDebInfo:
/Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/robot:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/robot


PostBuild.warehouse_main.RelWithDebInfo:
/Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/warehouse_main:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/warehouse_main


PostBuild.warehouse_ui.RelWithDebInfo:
/Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/warehouse_ui:
	/bin/rm -f /Users/tomlee/Documents/GitHub/Amaz00m/build/RelWithDebInfo/warehouse_ui




# For each target create a dummy ruleso the target does not have to exist
