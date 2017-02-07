cd
mkdir -p sources
cd sources
wget http://www.openframeworks.cc/versions/v0.8.0/of_v0.8.0_linux64_release.tar.gz
tar xzvf of_v0.8.0_linux64_release.tar.gz
cd of_v0.8.0_linux64_release/scripts/linux/ubuntu
sudo bash install_dependencies.sh
# make sure it install gstreamer1.0
# if not, edit the script to force gstreamer1.0 install
sudo bash install_codecs.sh
sed -i 's/freetype2\/freetype/freetype/' ../../../libs/openFrameworks/graphics/ofTrueTypeFont.cpp
cd ../../../apps
mkdir soixante
cd soixante
git clone git@github.com:emmanuelgeoffray/openTSPS.git
cd openTSPS
bash clone_addons.sh
sudo install_dependencies.sh
cd ../../../addons/ofxCv/
git reset --hard ac235ab
cd ../ofxIpVideoGrabber
git reset --hard 8f0775dc988175003d0aeb6d3795e6ff94293235
cd ../ofxHTTP
git reset --hard 8a5a51c1d7b0eb7a1a57ac2d6d04531ef27bb9de
cd ../ofxTaskQueue
git reset --hard 1a49a08daadbce46f67b4cfe65904eff76d76f06
cd ../ofxIO
git reset --hard 5861979ccf7d9053d2940330d7a5f2aa9fa90587
cd ../ofxMediaType
git reset --hard 764010240efd30531f4d70c103bad3e6615615ff
cd ../ofxSSLManager
git reset --hard 0acbc4bea436b63748781df971f88a3c9a2d4875
cd ../ofxSpacebrew
git reset --hard d99b734483de9af0f3b1d7c01670105a9bae544
cd ../../apps/soixante/openTSPS/standalone/basic
make
make run

