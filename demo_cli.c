#include "demo.h"

void cli_print_usage() {
    printf("Usage: gltf_view [path_to_gltf_file] (one or more options from below)\n");//[-in input_file] [-env hdr_file] [-aa ANTIALIAS_MODE] [-bg BACKGROUND_MODE]\n");
    printf("Options:\n");
    printf("  -in <input_file>         Specify the input file path.\n");
    printf("  -env <hdr_file>          Specify the environmental lighting / reflections HDR file path.\n");
    printf("  -env <int value>         Specify the environmental lighting / reflections HDR preset number.\n");
    printf("                            [1] = Footprint Court  [2] = Helipad       [3] = Field            \n");
    printf("                            [4] = Papermill        [5] = Pisa          [6] = Doge2            \n");
    printf("                            [7] = Ennis            [8] = Directional   [9] = Chromatic        \n");
    printf("                            [10] = Neutral\n");
    printf("  -env_pow <int value>     The environmental lighting brightness, default = 160 (160%% normal)\n");
    printf("  -expo <int value>        The exposure level, default = 80 (80%% normal)\n");
    printf("  -aa <mode>               Set antialiasing mode (ANTIALIAS_OFF [0], ANTIALIAS_CONSTANT [1], ANTIALIAS_NOT_MOVING [2]).\n");
    printf("  -bg <mode>               Set background mode (BG_CLEAR [0], BG_SOLID [1], BG_ENVIRONMENT [2]).\n");
    printf("  -blur_bg <int value>     How much to blur the background between 0 and 1000.\n");
    printf("  -pitch <int value>       Viewing angle pitch in degrees x 10.\n");
    printf("  -yaw <int value>         Viewing angle yaw in degrees x 10.\n");
    printf("  -distance <int value>    Viewing distance where the default of 1000 = 1x the model bounding size.\n");
    printf("  -anim <int value>        Which animation number to play, or -1 to explicitly disable animation.\n");
    printf("  -anim_rate <int value>   How fast to play back the animation where 1000 is 1x normal rate.\n");
    printf("  -frame_count <int value> If this is specified and greater than zero, that many frames will be output as png files into\n");
    printf("                           the render_frames subdirectory, and then the program will shutdown.  See the __compile_clip.sh\n");
    printf("                           script for more information and an example of using ffmpeg to turn the frames into an mp4.\n");
    printf("  -sw                      Software Rendering mode (this is more compatible but far slower than using the GPU).\n");
}

bool demo_cli_apply_commandline_options( pViewer viewer, char * gltfFile, char * hdrFile, int * frame_count, bool * software_only, float * _anim_rate, int argc, char *argv[] ){
    bool gotFilenameInput = false;
    bool passedParamChecks = true;
    
    gltfFile[0] = '\0';
    
    // Check if at least one argument is provided
    if (argc < 2) {
        cli_print_usage();
        passedParamChecks = false;
    } else {
        // Get the glTF file path (first argument)
        int _first_param = 1;
        if (argc > 1) {
            if (argv[1][0] != '-') { 
                strncpy(gltfFile, argv[1], MAX_PATH_LENGTH - 1);
                gltfFile[MAX_PATH_LENGTH - 1] = '\0'; // Ensure null-termination
                _first_param = 2;
                gotFilenameInput = true;
            }
        }

        // Parse additional arguments
        for (int i = _first_param; i < argc; i++) {
            if (strcmp(argv[i], "-in") == 0 && (i + 1) < argc) {
                strncpy(gltfFile, argv[i + 1], MAX_PATH_LENGTH - 1);
                gltfFile[MAX_PATH_LENGTH - 1] = '\0'; // Ensure null-termination
                gotFilenameInput = true;
                i++; // Skip the next argument
            } else if (strcmp(argv[i], "-env") == 0 && (i + 1) < argc) {
                int p = atoi(argv[i + 1]);
                if (p == 0) {
                    strncpy(hdrFile, argv[i + 1], MAX_PATH_LENGTH - 1);
                } else {
                    if (p == 1) strncpy(hdrFile, "media/hdr/footprint_court.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 2) strncpy(hdrFile, "media/hdr/helipad.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 3) strncpy(hdrFile, "media/hdr/field.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 4) strncpy(hdrFile, "media/hdr/papermill.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 5) strncpy(hdrFile, "media/hdr/pisa.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 6) strncpy(hdrFile, "media/hdr/doge2.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 7) strncpy(hdrFile, "media/hdr/ennis.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 8) strncpy(hdrFile, "media/hdr/directional.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 9) strncpy(hdrFile, "media/hdr/chromatic.jpg", MAX_PATH_LENGTH - 1);
                    else if (p == 10) strncpy(hdrFile, "media/hdr/neutral.jpg", MAX_PATH_LENGTH - 1);
                }
                hdrFile[MAX_PATH_LENGTH - 1] = '\0';
                i++;
            } else if (strcmp(argv[i], "-aa") == 0 && (i + 1) < argc) {
                if ((strcmp(argv[i + 1], "ANTIALIAS_OFF") == 0) || (strcmp(argv[i + 1], "0") == 0)) {
                    lv_gltfview_set_aa_mode(viewer, ANTIALIAS_OFF );
                } else if ((strcmp(argv[i + 1], "ANTIALIAS_CONSTANT") == 0) || (strcmp(argv[i + 1], "1") == 0)) {
                    lv_gltfview_set_aa_mode(viewer, ANTIALIAS_CONSTANT );
                } else if ((strcmp(argv[i + 1], "ANTIALIAS_NOT_MOVING") == 0) || (strcmp(argv[i + 1], "2") == 0)) {
                    lv_gltfview_set_aa_mode(viewer, ANTIALIAS_NOT_MOVING );
                } else {
                    lv_gltfview_set_aa_mode(viewer, ANTIALIAS_NOT_MOVING );
                }
                i++;
            } else if (strcmp(argv[i], "-bg") == 0 && (i + 1) < argc) {
                if ((strcmp(argv[i + 1], "BG_CLEAR") == 0) || (strcmp(argv[i + 1], "0") == 0)) {
                    lv_gltfview_set_bg_mode(viewer, BG_CLEAR);
                } else if ((strcmp(argv[i + 1], "BG_SOLID") == 0) || (strcmp(argv[i + 1], "1") == 0)) {
                    lv_gltfview_set_bg_mode(viewer, BG_SOLID);
                } else if ((strcmp(argv[i + 1], "BG_ENVIRONMENT") == 0) || (strcmp(argv[i + 1], "2") == 0)) {
                    lv_gltfview_set_bg_mode(viewer, BG_ENVIRONMENT);
                } else {
                    lv_gltfview_set_bg_mode(viewer, BG_CLEAR);
                }
                i++;
            } else if (strcmp(argv[i], "-blur_bg") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_blur_bg(viewer, (float)(atoi(argv[i + 1])) / 1000.f );
                    i++;
                } else {
                    printf("Error: -blur_bg option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-env_pow") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_env_pow(viewer, atoi(argv[i + 1]) / 100.0f );
                    i++;
                } else {
                    printf("Error: -env_pow option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-expo") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_exposure(viewer, atoi(argv[i + 1]) / 100.0f );
                    i++;
                } else {
                    printf("Error: -expo option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-sw") == 0) {
                *software_only = true;
            } else if (strcmp(argv[i], "-desktop") == 0) {
                if (demo_os_integrate_confirm_desktop_mode_ok()) {
                    desktop_mode = true;                
                } else {
                    printf("Error: -desktop option requires an additional setup step, see below:\n");
                    printf("To avoid excessive wear and tear on the SD card or storage medium, the \n");
                    printf("desktop output mode saves it's temporary files to a ram drive.  That \n");
                    printf("ramdrive was not detected at this time.  To create it, run the following\n");
                    printf("script from the application root directory:\n\n");
                    char command[256];
                    snprintf(command, sizeof(command), "./__resize_ramdrive.sh %s %s\n", DESKTOP_OUTPUT_RAMTEMP_PATH, DESKTOP_OUTPUT_RAMTEMP_SIZE);
                    printf(command);
                    passedParamChecks = false;
                }
            } else if (strcmp(argv[i], "-anim") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    anim = atoi(argv[i + 1]); // Convert string to int
                    if (anim < 0) {
                        anim_enabled = false;
                        anim = 0;
                    }
                    i++;
                } else {
                    printf("Error: -expo option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-frame_count") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    *frame_count = atoi(argv[i + 1]); 
                    i++; 
                } else {
                    printf("Error: -frame_count option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-pitch") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_pitch(viewer, atoi(argv[i + 1]));
                    i++;
                } else {
                    printf("Error: -pitch option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-yaw") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_yaw(viewer, atoi(argv[i + 1]));
                    i++; 
                } else {
                    printf("Error: -yaw option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-spin_rate") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    spin_rate = atoi(argv[i + 1]) / 10.0f;
                    i++;
                } else {
                    printf("Error: -spin_rate option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-anim_rate") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    *_anim_rate = atoi(argv[i + 1]) / 1000.0f;
                    i++;
                } else {
                    printf("Error: -anim_rate option requires an integer value.\n");
                }
            } else if (strcmp(argv[i], "-distance") == 0 && (i + 1) < argc) {
                if (i + 1 < argc) {
                    lv_gltfview_set_distance(viewer, atoi(argv[i + 1]));

                    i++;
                } else {
                    printf("Error: -distance option requires an integer value.\n");
                }                
            } else {
                printf("Unknown option: %s\n", argv[i]);
                cli_print_usage();
                passedParamChecks = false;
            }
        } 
    }

    passedParamChecks &= gotFilenameInput;
    return passedParamChecks;
}