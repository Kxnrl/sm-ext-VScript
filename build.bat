echo "Set environment"
set EXT_DIR=%cd%
set DEP_DIR=%EXT_DIR%\build-windows
set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
mkdir "%DEP_DIR%"

cd %DEP_DIR%

if "%VSCMD_VER%"=="" (
	set MAKE=
	set CC=
	set CXX=
	call %VCVARSALL% x86
)

:: Install ambuild
echo "Install ambuild"
git clone https://github.com/alliedmodders/ambuild
pushd ambuild
python setup.py install
popd

:: Getting sourcemod

echo "Download sourcemod"
git clone https://github.com/alliedmodders/sourcemod --recursive --branch %SMBRANCH% --single-branch sourcemod

pushd sourcemod
set SOURCEMOD=%cd%
popd

:: Getting metamod

echo "Download metamod"
git clone https://github.com/alliedmodders/metamod-source --recursive --branch %MMBRANCH% --single-branch metamod

pushd metamod
set METAMOD=%cd%
popd

:: Getting hl2sdk

echo "Download hl2sdk-csgo"
git clone https://github.com/alliedmodders/hl2sdk --recursive --branch csgo --single-branch hl2sdk-csgo

:: Start build

echo "Build"
cd %EXT_DIR%

mkdir build
pushd build
python "%EXT_DIR%/configure.py" --enable-optimize --sm-path "%SOURCEMOD%" --mms-path "%METAMOD%" --hl2sdk-root "%DEP_DIR%" --sdks=csgo
ambuild
:: Assumption: after_success will return to the root dir
dir
:: check linux ver build
:: popd
