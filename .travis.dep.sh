# Preparing the cache
if [ ! -f "$HOME/cache/cortex-m.tar.bz2" ]; then
	echo "Downloading Cortex-M toolchain..."
	(cd $HOME/cache && wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 -O cortex-m.tar.bz2) &
fi
# wait for all downloads to finish
wait
echo "Downloads done."

# unzip all toolchains
mkdir $HOME/toolchain
echo "Expanding Cortex-M toolchain..."
(cd $HOME/toolchain && tar -xjf $HOME/cache/cortex-m.tar.bz2) &
# synchronize
wait
echo "Expanding done."
