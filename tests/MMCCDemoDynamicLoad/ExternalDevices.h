#pragma once

// External devices abstract interface

// from cpx to outside world
struct Container {
   int initialize(const char* external_configuration);
   int shutdown();

   int start_stage();
   int stop_stage();
   int stage_get_position(double* pos_um);

   int light_source_set(int channel, int state);
   int light_source_get(int channel, int* state);

   // ... other device actions
};
