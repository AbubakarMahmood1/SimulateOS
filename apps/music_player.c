/**
 * music_player.c - Music Player Application
 * Full-featured music player with threaded playback simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_SONGS 50
#define MAX_TITLE_LEN 100

typedef enum {
    STOPPED,
    PLAYING,
    PAUSED
} PlayerState;

typedef struct {
    char title[MAX_TITLE_LEN];
    char artist[MAX_TITLE_LEN];
    int duration_sec;  // Song duration in seconds
} Song;

Song playlist[MAX_SONGS];
int playlist_size = 0;
int current_track = 0;
int current_position = 0;  // Current position in seconds
PlayerState state = STOPPED;
int volume = 70;
int repeat_mode = 0;  // 0=off, 1=repeat all, 2=repeat one
int shuffle_mode = 0;

pthread_mutex_t player_mutex;
pthread_t playback_thread;
int thread_running = 0;

/**
 * playback_worker - Thread that simulates playback
 */
void* playback_worker(void* arg) {
    (void)arg;

    while (thread_running) {
        pthread_mutex_lock(&player_mutex);

        if (state == PLAYING && playlist_size > 0) {
            Song* current = &playlist[current_track];

            current_position++;

            // Check if song finished
            if (current_position >= current->duration_sec) {
                current_position = 0;

                if (repeat_mode == 2) {
                    // Repeat current song
                    current_position = 0;
                } else {
                    // Move to next song
                    current_track++;

                    if (current_track >= playlist_size) {
                        if (repeat_mode == 1) {
                            current_track = 0;  // Repeat playlist
                        } else {
                            current_track = playlist_size - 1;
                            state = STOPPED;
                            current_position = 0;
                        }
                    }
                }
            }
        }

        pthread_mutex_unlock(&player_mutex);
        sleep(1);
    }

    return NULL;
}

/**
 * add_song - Add a song to playlist
 */
void add_song(const char* title, const char* artist, int duration) {
    if (playlist_size >= MAX_SONGS) {
        printf("Playlist is full!\n");
        return;
    }

    Song* song = &playlist[playlist_size];
    strncpy(song->title, title, MAX_TITLE_LEN - 1);
    strncpy(song->artist, artist, MAX_TITLE_LEN - 1);
    song->duration_sec = duration;

    playlist_size++;
    printf("Added: %s - %s (%d:%02d)\n",
           artist, title, duration / 60, duration % 60);
}

/**
 * remove_song - Remove song from playlist
 */
void remove_song(int index) {
    if (index < 0 || index >= playlist_size) {
        printf("Invalid song index.\n");
        return;
    }

    printf("Removed: %s - %s\n",
           playlist[index].artist, playlist[index].title);

    // Shift remaining songs
    for (int i = index; i < playlist_size - 1; i++) {
        playlist[i] = playlist[i + 1];
    }

    playlist_size--;

    // Adjust current track if needed
    if (current_track >= playlist_size && playlist_size > 0) {
        current_track = playlist_size - 1;
    }
    if (playlist_size == 0) {
        current_track = 0;
        state = STOPPED;
    }
}

/**
 * format_time - Format seconds as MM:SS
 */
void format_time(int seconds, char* buffer, size_t size) {
    snprintf(buffer, size, "%d:%02d", seconds / 60, seconds % 60);
}

/**
 * print_progress_bar - Display playback progress
 */
void print_progress_bar(void) {
    if (playlist_size == 0) return;

    Song* current = &playlist[current_track];
    int percent = (current_position * 100) / current->duration_sec;
    int bars = (current_position * 30) / current->duration_sec;

    char pos_str[16], dur_str[16];
    format_time(current_position, pos_str, sizeof(pos_str));
    format_time(current->duration_sec, dur_str, sizeof(dur_str));

    printf("  [");
    for (int i = 0; i < 30; i++) {
        if (i < bars) printf("═");
        else printf("─");
    }
    printf("] %s / %s (%d%%)\n", pos_str, dur_str, percent);
}

/**
 * print_player_status - Display current player status
 */
void print_player_status(void) {
    const char* state_str[] = {"STOPPED", "PLAYING", "PAUSED"};
    const char* repeat_str[] = {"Off", "All", "One"};

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      MUSIC PLAYER STATUS                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    pthread_mutex_lock(&player_mutex);

    if (playlist_size == 0) {
        printf("║  Playlist: Empty                                             ║\n");
    } else {
        Song* current = &playlist[current_track];
        printf("║  Now Playing: %-47s║\n",
               state == STOPPED ? "---" :
               (char[48]){0} + snprintf((char[48]){0}, 48, "%s - %s",
                                       current->artist, current->title));
        printf("║  Track: %2d / %-2d                                             ║\n",
               current_track + 1, playlist_size);
        printf("║  Status: %-52s║\n", state_str[state]);

        printf("╠══════════════════════════════════════════════════════════════╣\n");

        if (state != STOPPED) {
            print_progress_bar();
        }
    }

    printf("║                                                              ║\n");
    printf("║  Volume: %-3d%%  Repeat: %-5s  Shuffle: %-3s               ║\n",
           volume, repeat_str[repeat_mode], shuffle_mode ? "On" : "Off");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    pthread_mutex_unlock(&player_mutex);
    printf("\n");
}

/**
 * print_playlist - Display playlist
 */
void print_playlist(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                       PLAYLIST                            \n");
    printf("═══════════════════════════════════════════════════════════\n");

    if (playlist_size == 0) {
        printf("  (empty)\n");
    } else {
        for (int i = 0; i < playlist_size; i++) {
            char dur_str[16];
            format_time(playlist[i].duration_sec, dur_str, sizeof(dur_str));

            if (i == current_track) {
                printf("  ▶ %2d. %-25s - %-25s [%s]\n",
                       i + 1, playlist[i].artist, playlist[i].title, dur_str);
            } else {
                printf("    %2d. %-25s - %-25s [%s]\n",
                       i + 1, playlist[i].artist, playlist[i].title, dur_str);
            }
        }
    }

    printf("═══════════════════════════════════════════════════════════\n\n");
}

/**
 * print_help - Display help
 */
void print_help(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                    PLAYER COMMANDS                        \n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Playback:\n");
    printf("  play            - Start playback\n");
    printf("  pause           - Pause playback\n");
    printf("  stop            - Stop playback\n");
    printf("  next            - Next track\n");
    printf("  prev            - Previous track\n");
    printf("\n");
    printf("Playlist:\n");
    printf("  list            - Show playlist\n");
    printf("  add             - Add song (interactive)\n");
    printf("  remove <num>    - Remove song by number\n");
    printf("  jump <num>      - Jump to track number\n");
    printf("\n");
    printf("Settings:\n");
    printf("  volume <0-100>  - Set volume\n");
    printf("  repeat          - Cycle repeat mode (off/all/one)\n");
    printf("  shuffle         - Toggle shuffle\n");
    printf("\n");
    printf("Other:\n");
    printf("  status          - Show player status\n");
    printf("  help            - Show this help\n");
    printf("  exit            - Exit player\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
}

/**
 * main - Music Player entry point
 *
 * Full music player with threaded playback simulation, playlist
 * management, and standard controls.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[256];
    char command[64];
    int arg;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                      MUSIC PLAYER                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Features: Threaded playback, playlist, controls\n");
    printf("════════════════════════════════════════════════════════════\n");

    // Initialize mutex
    pthread_mutex_init(&player_mutex, NULL);

    // Add some default songs
    add_song("Bohemian Rhapsody", "Queen", 354);
    add_song("Stairway to Heaven", "Led Zeppelin", 482);
    add_song("Imagine", "John Lennon", 183);
    add_song("Hotel California", "Eagles", 391);
    add_song("Sweet Child O' Mine", "Guns N' Roses", 356);
    add_song("Smells Like Teen Spirit", "Nirvana", 301);
    add_song("Billie Jean", "Michael Jackson", 294);
    add_song("Hey Jude", "The Beatles", 431);

    printf("\n");
    printf("Welcome to MiniOS Music Player!\n");
    printf("Type 'help' for available commands\n");

    // Start playback thread
    thread_running = 1;
    pthread_create(&playback_thread, NULL, playback_worker, NULL);

    print_help();

    // Main command loop
    while (1) {
        print_player_status();

        printf("player> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Parse command
        if (sscanf(input, "%s %d", command, &arg) < 1) {
            continue;
        }

        // Process commands
        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            break;
        } else if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strcmp(command, "list") == 0) {
            print_playlist();
        } else if (strcmp(command, "status") == 0) {
            // Status is printed every loop
            continue;
        } else if (strcmp(command, "play") == 0) {
            pthread_mutex_lock(&player_mutex);
            if (playlist_size > 0) {
                state = PLAYING;
                printf("▶ Playing...\n");
            } else {
                printf("Playlist is empty!\n");
            }
            pthread_mutex_unlock(&player_mutex);
        } else if (strcmp(command, "pause") == 0) {
            pthread_mutex_lock(&player_mutex);
            if (state == PLAYING) {
                state = PAUSED;
                printf("⏸ Paused\n");
            } else {
                printf("Not playing.\n");
            }
            pthread_mutex_unlock(&player_mutex);
        } else if (strcmp(command, "stop") == 0) {
            pthread_mutex_lock(&player_mutex);
            state = STOPPED;
            current_position = 0;
            printf("⏹ Stopped\n");
            pthread_mutex_unlock(&player_mutex);
        } else if (strcmp(command, "next") == 0) {
            pthread_mutex_lock(&player_mutex);
            if (playlist_size > 0) {
                current_track = (current_track + 1) % playlist_size;
                current_position = 0;
                printf("⏭ Next track\n");
            }
            pthread_mutex_unlock(&player_mutex);
        } else if (strcmp(command, "prev") == 0) {
            pthread_mutex_lock(&player_mutex);
            if (playlist_size > 0) {
                current_track = (current_track - 1 + playlist_size) % playlist_size;
                current_position = 0;
                printf("⏮ Previous track\n");
            }
            pthread_mutex_unlock(&player_mutex);
        } else if (strcmp(command, "jump") == 0) {
            if (sscanf(input, "%s %d", command, &arg) == 2) {
                pthread_mutex_lock(&player_mutex);
                if (arg >= 1 && arg <= playlist_size) {
                    current_track = arg - 1;
                    current_position = 0;
                    printf("Jumped to track %d\n", arg);
                } else {
                    printf("Invalid track number (1-%d)\n", playlist_size);
                }
                pthread_mutex_unlock(&player_mutex);
            } else {
                printf("Usage: jump <track_number>\n");
            }
        } else if (strcmp(command, "volume") == 0) {
            if (sscanf(input, "%s %d", command, &arg) == 2) {
                if (arg >= 0 && arg <= 100) {
                    volume = arg;
                    printf("Volume set to %d%%\n", volume);
                } else {
                    printf("Volume must be 0-100\n");
                }
            } else {
                printf("Usage: volume <0-100>\n");
            }
        } else if (strcmp(command, "repeat") == 0) {
            repeat_mode = (repeat_mode + 1) % 3;
            const char* modes[] = {"Off", "All", "One"};
            printf("Repeat: %s\n", modes[repeat_mode]);
        } else if (strcmp(command, "shuffle") == 0) {
            shuffle_mode = !shuffle_mode;
            printf("Shuffle: %s\n", shuffle_mode ? "On" : "Off");
        } else if (strcmp(command, "add") == 0) {
            char title[MAX_TITLE_LEN], artist[MAX_TITLE_LEN];
            int minutes, seconds;

            printf("\nAdd new song:\n");
            printf("  Title: ");
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;

            printf("  Artist: ");
            fgets(artist, sizeof(artist), stdin);
            artist[strcspn(artist, "\n")] = 0;

            printf("  Duration (MM:SS): ");
            if (scanf("%d:%d", &minutes, &seconds) == 2) {
                getchar();  // consume newline
                add_song(title, artist, minutes * 60 + seconds);
            } else {
                printf("Invalid duration format\n");
                while (getchar() != '\n');  // clear input
            }
        } else if (strcmp(command, "remove") == 0) {
            if (sscanf(input, "%s %d", command, &arg) == 2) {
                pthread_mutex_lock(&player_mutex);
                remove_song(arg - 1);
                pthread_mutex_unlock(&player_mutex);
            } else {
                printf("Usage: remove <song_number>\n");
            }
        } else {
            printf("Unknown command '%s'. Type 'help' for available commands.\n", command);
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    // Cleanup
    thread_running = 0;
    pthread_join(playback_thread, NULL);
    pthread_mutex_destroy(&player_mutex);

    printf("\nMusic Player closing...\n");
    printf("Thanks for listening! ♫\n");

    return 0;
}
