$installerSourcePath = Split-Path -parent $PSCommandPath
heat dir $pwd -o "$installerSourcePath\files.wxs" -gg -cg ApplicationFiles -var sys.CURRENTDIR -dr INSTALLDIR -srd
candle -o "$installerSourcePath\" "$installerSourcePath\*.wxs"
light -ext WixUIExtension -ext WixUtilExtension -o "$installerSourcePath\talkie.msi" "$installerSourcePath\*.wixobj"