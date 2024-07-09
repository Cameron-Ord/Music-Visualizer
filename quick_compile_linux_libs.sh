cd ~/Projects/Music-Visualizer/

echo "Enter cores to utilize --max 16-- : "
read cores

working_dir=$(pwd)

if [[ $cores =~ ^[0-9]+$ ]]; then
	if [ $cores -lt 1 ]; then
		$cores=1
	fi

	if [ $cores -gt 16 ]; then
		$cores=16
	fi
else
	echo "Must enter a number!"
	exit
fi

if [ ! -d "linux_resources" ]; then
	echo "Fetch the resources first!"
	exit
fi

cd ${working_dir}/linux_resources

if [ ! -d "SDL2" ]; then
	mkdir SDL2
	mkdir SDL2/include
	mkdir SDL2/include/SDL2
else
	rm -r SDL2
	mkdir SDL2
	mkdir SDL2/include
	mkdir SDL2/include/SDL2
fi

if [ ! -d "libsndfile" ]; then
	mkdir libsndfile
	mkdir libsndfile/include
else
	rm -r libsndfile
	mkdir libsndfile
	mkdir libsndfile/include
fi

LIB_SND_FILE_VER="1.2.2"
SDL2_VER="2.30.5"
SDL2_IMG_VER="2.8.2"
SDL2_TTF_VER="2.22.0"

LINSDL2="SDL2-${SDL2_VER}"
LINSDLIMG="SDL2_image-${SDL2_IMG_VER}"
LINSDLTTF="SDL2_ttf-${SDL2_TTF_VER}"
LINSND="libsndfile-${LIB_SND_FILE_VER}"

echo "COMPILILING SDL2"

if [ -d $LINSDL2 ]; then
	cd $LINSDL2
	./configure
	make -j${core_int}
	cd $working_dir/linux_resources
	cp -r ${LINSDL2}/include/*.h SDL2/include/SDL2/
fi

echo "COMPILILING IMAGE"

if [ -d $LINSDLIMG ]; then
	cd $LINSDLIMG
	./autogen.sh
	./configure
	make -j${core_int}
	cd $working_dir/linux_resources
	cp -r ${LINSDLIMG}/include/*.h SDL2/include/SDL2/
fi

echo "COMPILILING TTF"numeric representations.

if [ -d $LINSDLTTF ]; then
	cd $LINSDLTTF
	./autogen.sh
	./configure
	make -j${core_int}
	cd $working_dir/linux_resources
	cp -r ${LINSDLTTF}/*.h SDL2/include/SDL2/
fi

echo "COMPILING LIBSND"

if [ -d $LINSND ]; then
	cd $LINSND
	./configure
	make -j${core_int}
	cd $working_dir/linux_resources
	cp -r ${LINSND}/include/*.h libsndfile/include/
fi
