#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space_explorer.h"

#define DIST_THRESHOLD  1.0

// Function prototype for get_distance
double get_distance(Point a, Point b);

ShipAction space_hop(unsigned int crt_planet,
                     unsigned int *connections,
                     int num_connections,
                     double distance_from_mixer,
                     void *ship_state,
                     Planet *planets) { 
                         
    // Cast ship state to the appropriate data structure
    ShipState *state = (ShipState*)ship_state;

    // Initialize ship state if it's the first call
    if (state == NULL) {
        state = malloc(sizeof(ShipState));
        if (state == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        // Initialize state variables as needed
        state->visited = malloc(num_connections * sizeof(int));
        if (state->visited == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        // Initialize visited array with invalid planet IDs
        for (int i = 0; i < num_connections; i++) {
            state->visited[i] = -1;
        }
        state->prev_planet = -1; // Initialize previous planet to an invalid value
    }
    
    // Update ship state with current planet
    if (state != NULL && crt_planet >= 0 && crt_planet < num_connections) {
        state->visited[crt_planet] = crt_planet;
    }
    
    // Find the next unvisited planet with the minimum distance to the mixer
    unsigned int next_planet = -1; // Initialize next planet to an invalid value
    double min_distance = INFINITY;
    for (int i = 0; i < num_connections; i++) {
        unsigned int candidate = connections[i];
        if (state != NULL && candidate >= 0 && candidate < num_connections && state->visited[candidate] == -1) {
            double candidate_distance = get_distance(planets[crt_planet].point, planets[candidate].point);
            if (candidate_distance < min_distance) {
                min_distance = candidate_distance;
                next_planet = candidate;
            }
        }
    }

    // If all connected planets have been visited, backtrack to the previous planet
    if (next_planet == -1) {
        if (state != NULL) {
            next_planet = state->prev_planet;
        }
    }

    // Update previous planet in ship state for backtracking
    if (state != NULL) {
        state->prev_planet = crt_planet;
    }

    // Free ship state if the particle mixer is found
    if (state != NULL && distance_from_mixer < DIST_THRESHOLD) {
        free(state->visited);
        free(state);
        return (ShipAction){RAND_PLANET, NULL}; // Return random planet to terminate exploration
    }

    // Return the next planet and updated ship state
    return (ShipAction){next_planet, state};
}

