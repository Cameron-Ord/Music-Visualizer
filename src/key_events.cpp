#include "../include/events.hpp"

SDL2KeyInputs::SDL2KeyInputs()
{
    cursor_index_dirs = 0;
    cursor_index_songs = 0;
}

void SDL2KeyInputs::reset_cursor_index(size_t *cursor_index_ptr)
{
    *cursor_index_ptr = 0;
}

size_t *SDL2KeyInputs::get_song_cursor_index()
{
    return &cursor_index_songs;
}

size_t *SDL2KeyInputs::get_dir_cursor_index()
{
    return &cursor_index_dirs;
}

void SDL2KeyInputs::cycle_up_list(size_t max_length, size_t *cursor_index_ptr)
{
    int tmp = static_cast<int>(*cursor_index_ptr);
    int length = static_cast<int>(max_length);

    if (length - 1 < 0)
    {
        return;
    }

    tmp -= 1;
    if (tmp < 0)
    {
        tmp = length - 1;
    }

    *cursor_index_ptr = static_cast<size_t>(tmp);
}

void SDL2KeyInputs::cycle_down_list(size_t max_length, size_t *cursor_index_ptr)
{
    int tmp = static_cast<int>(*cursor_index_ptr);
    int length = static_cast<int>(max_length);

    if (length - 1 < 0)
    {
        return;
    }

    tmp += 1;
    if (tmp > length - 1)
    {
        tmp = 0;
    }

    *cursor_index_ptr = static_cast<size_t>(tmp);
}

std::string SDL2KeyInputs::select_element(const std::vector<Text> *d,
                                          size_t *cursor_index_ptr)
{
    size_t vec_size = d->size() - 1;

    if (vec_size < 0)
    {
        return "<empty-vector>";
    }

    if (*cursor_index_ptr > vec_size)
    {
        *cursor_index_ptr = vec_size;
    }

    // Since type is size_t, it can't go below 0.

    return (*d)[*cursor_index_ptr].name;
}
