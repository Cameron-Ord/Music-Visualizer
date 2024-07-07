if [ ! -d "tempdump" ]; then
	mkdir tempdump
fi

working_dir=$(pwd)

cd tempdump
tempdump_dir=$(pwd)

LIB_SND_FILE_VER="1.2.2"
SDL2_VER="2.30.5"
SDL2_IMG_VER="2.8.2"
SDL2_TTF_VER="2.22.0"

LINSDL2="SDL2-${SDL2_VER}"
LINSDLIMG="SDL2_image-${SDL2_IMG_VER}"
LINSDLTTF="SDL2_ttf-${SDL2_TTF_VER}"
LINSND="libsndfile-${LIB_SND_FILE_VER}"

WINSDL2="SDL2-devel-${SDL2_VER}-mingw"
WINSND="libsndfile-${LIB_SND_FILE_VER}-win64"
WINSDLIMG="SDL2_image-devel-${SDL2_IMG_VER}-mingw"
WINSDLTTF="SDL2_ttf-devel-${SDL2_TTF_VER}-mingw"

if [ "$tempdump_dir" == "${working_dir}/tempdump" ]; then

	if [ ! -d "win64" ]; then
		mkdir win64
	fi

	if [ ! -d "linux64" ]; then
		mkdir linux64
	fi

	cd linux64
	linux_path=$(pwd)

	buffer=("$linux_path"/*)
	for item in "${buffer[@]}"; do
		if [ -f "$item" ]; then
			rm -f $item
		elif [ -d "$item" ]; then
			rm -r $item
		fi
	done

	wget "https://github.com/libsndfile/libsndfile/releases/download/1.2.2/${LINSND}.tar.xz"
	wget "https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/${LINSDL2}.zip"
	wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/${LINSDLIMG}.zip"
	wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/${LINSDLTTF}.zip"

	buffer=("$linux_path"/*)
	for item in "${buffer[@]}"; do
		if [ -f "$item" ]; then
			ext="${item##*.}"
			case $ext in
			"xz") tar -xvf $item ;;
			"zip") unzip $item ;;
			*) ;;
			esac

			rm -f $item
		fi
	done

	cd ..
	cd win64
	win_path=$(pwd)

	buffer=("$win_path"/*)
	for item in "${buffer[@]}"; do
		if [ -f "$item" ]; then
			rm -f $item
		elif [ -d "$item" ]; then
			rm -r $item
		fi
	done

	wget "https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/${WINSDL2}.zip"
	wget "https://github.com/libsndfile/libsndfile/releases/download/1.2.2/${WINSND}.zip"
	wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/${WINSDLIMG}.zip"
	wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/${WINSDLTTF}.zip"

	buffer=("$win_path"/*)
	for item in "${buffer[@]}"; do
		if [ -f "$item" ]; then
			ext="${item##*.}"
			case $ext in
			"xz") tar -xvf $item ;;
			"zip") unzip $item ;;
			*) ;;
			esac

			rm -f $item
		fi
	done

	cd ..
	cd linux64
	buffer=("$linux_path"/*)

	if [ ! -d ${working_dir}/"linux_resources" ]; then
		mkdir ${working_dir}/linux_resources
	else
		rm -r ${working_dir}/linux_resources
		mkdir ${working_dir}/linux_resources
	fi

	current_dir=$(pwd)

	for item in "${buffer[@]}"; do
		if [ -d "$item" ]; then
			mv $item ${working_dir}/"linux_resources/"
		fi
	done

	echo "Compile libs? : y/n"
	read resp

	if [ $resp == "y" ]; then

		echo "Enter cores to utilize --max 16-- : "
		read cores

		let core_int=cores

		if [ $core_int -lt 1 ]; then
			$core_int=1
		fi

		if [ $core_int -gt 16 ]; then
			$core_int=16
		fi

		cd ${working_dir}/linux_resources

		mkdir SDL2
		mkdir SDL2/include
		mkdir SDL2/include/SDL2

		mkdir libsndfile
		mkdir libsndfile/include

		cd $LINSND

		./configure
		make -j${core_int}

		cd ..
		cd $LINSDL2

		./autogen.sh
		./configure
		make -j${core_int}

		cd ..
		cd $LINSDLIMG

		./autogen.sh
		./configure
		make -j${core_int}

		cd ..
		cd $LINSDLTTF

		./autogen.sh
		./configure
		make -j${core_int}

		cd ..

		cp -r ${LINSND}/include/*.h libsndfile/include/
		cp -r ${LINSDL2}/include/*.h SDL2/include/SDL2/
		cp -r ${LINSDLIMG}/include/*.h SDL2/include/SDL2/
		cp -r ${LINSDLTTF}/*.h SDL2/include/SDL2/

		cp -r ${LINSDL2}/build/.libs/libSDL2-2.0.so.0.3000.5 SDL2/
		cp -r ${LINSDLIMG}/.libs/libSDL2_image-2.0.so.0.800.2 SDL2/
		cp -r ${LINSDLTTF}/.libs/libSDL2_ttf-2.0.so.0.2200.0 SDL2/
		cp -r ${LINSND}/src/.libs/libsndfile.so.1.0.37 libsndfile/

	fi

	cd $current_dir

	cd ..
	cd win64
	buffer=("$win_path"/*)

	if [ ! -d ${working_dir}/"win_resources" ]; then
		mkdir ${working_dir}/win_resources
	else
		rm -r ${working_dir}/win_resources
		mkdir ${working_dir}/win_resources
	fi

	mkdir x86_64-w64-mingw32

	for item in "${buffer[@]}"; do
		if [ -d "$item" ]; then
			mv $item ${working_dir}/"win_resources/"
		fi
	done
fi

cd $working_dir

if [ -d "tempdump" ]; then
	rm -r tempdump
fi
