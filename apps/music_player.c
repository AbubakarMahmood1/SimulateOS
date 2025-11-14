/**
 * music_player.c - Music Player Application
 * Background audio player with playlist
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * main - Music Player entry point
 *
 * Simulates a music player running in the background.
 *
 * Returns: 0 on success
 */
int main(void) {
    const char* songs[] = {
        "Bohemian Rhapsody - Queen",
        "Stairway to Heaven - Led Zeppelin",
        "Imagine - John Lennon",
        "Hotel California - Eagles",
        "Sweet Child O' Mine - Guns N' Roses"
    };
    int num_songs = 5;
    int current_song = 0;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                      MUSIC PLAYER                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process (Background)\n");
    printf("  Priority: MEDIUM (Background Task)\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Playlist (%d songs):\n", num_songs);
    printf("─────────────────────────────────────────────────────────────\n");
    for (int i = 0; i < num_songs; i++) {
        if (i == current_song) {
            printf("  ▶ %s\n", songs[i]);
        } else {
            printf("    %s\n", songs[i]);
        }
    }
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Now Playing: %s\n", songs[current_song]);
    printf("\n");
    printf("♪ ♫ ♪ ♫  Playing in background...  ♫ ♪ ♫ ♪\n");
    printf("\n");

    printf("Simulating playback...\n");
    for (int i = 0; i < 5; i++) {
        printf("♪ ");
        fflush(stdout);
        sleep(1);
    }
    printf("\n\n");

    printf("Controls: play, pause, next, prev, exit\n");
    char command[20];
    printf("Command: ");
    fgets(command, sizeof(command), stdin);

    printf("Music Player closing...\n");

    return 0;
}
