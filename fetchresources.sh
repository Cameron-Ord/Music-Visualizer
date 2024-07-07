if [ ! -d "tempdump" ]; then
	mkdir tempdump
fi

working_dir=$(pwd)

cd tempdump
tempdump_dir=$(pwd)

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

	LIB_SND_FILE_VER="libsndfile-1.2.2"
	SDL2_VER="SDL2-2.30.5"
	SDL2_IMG_VER="SDL2_image-2.8.2"
	SDL2_TTF_VER="SDL2_ttf-2.22.0"

	wget "https://github.com/libsndfile/libsndfile/releases/download/1.2.2/${LIB_SND_FILE_VER}.tar.xz"
	wget "https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/${SDL2_VER}.zip"
	wget "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/${SDL2_IMG_VER}.zip"
	wget "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/${SDL2_TTF_VER}.zip"

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

	wget https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/SDL2-devel-2.30.5-mingw.zip
	wget https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2-win64.zip
	wget https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-devel-2.8.2-mingw.zip
	wget https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.zip

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
		cd $LIB_SND_FILE_VER

		./configure
		make -j${core_int}

		cd ..
		cd $SDL2_VER

		./autogen.sh
		./configure
		make -j${core_int}

		cd ..
		cd $SDL2_IMG_VER

		./autogen.sh
		./configure
		make -j${core_int}

		cd ..
		cd $SDL2_TTF_VER

		./autogen.sh
		./configure
		make -j${core_int}

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
