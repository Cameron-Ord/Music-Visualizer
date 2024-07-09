cd ~/Projects/Music-Visualizer/
working_dir=$(pwd)

if [ ! -d "tempdump" ]; then
	mkdir tempdump
fi

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

if [ -d "${working_dir}/tempdump" ]; then

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
			rm $item
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

			rm $item
		fi
	done

	cd ..
	cd win64
	win_path=$(pwd)

	buffer=("$win_path"/*)
	for item in "${buffer[@]}"; do
		if [ -f "$item" ]; then
			rm $item
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

			rm $item
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

	cd $current_dir

	cd ..
	cd win64
	buffer=("$win_path"/*)

	if [ ! -d ${working_dir}/"win_resources" ]; then
		mkdir ${working_dir}/win_resources
	else
		rm -r ${working_dir}/win_resources
		mkdir ${working_dir}/win_resources
		mkdir ${working_dir}/win_resources/SDL_HEADERS
		mkdir ${working_dir}/win_resources/SDL_HEADERS/include
		mkdir ${working_dir}/win_resources/SDL_HEADERS/include/SDL2
		mkdir ${working_dir}/win_resources/LIBSNDFILE_HEADER
		mkdir ${working_dir}/win_resources/LIBSNDFILE_HEADER/include
	fi

	current_location=$(pwd)

	for item in "${buffer[@]}"; do
		if [ -d "$item" ]; then
			# If the path string contains libsndfile or sdl
			if [[ $item == *"libsndfile"* ]]; then
				cp -r ${item}/include/*.h ${working_dir}/win_resources/LIBSNDFILE_HEADER/include/
			elif [[ $item == *"SDL"* ]]; then
				cp -r ${item}/x86_64-w64-mingw32/include/SDL2/*.h ${working_dir}/win_resources/SDL_HEADERS/include/SDL2
			fi
			mv $item ${working_dir}/win_resources/
		fi
	done
fi

cd $working_dir

if [ -d "tempdump" ]; then
	rm -r tempdump
fi
