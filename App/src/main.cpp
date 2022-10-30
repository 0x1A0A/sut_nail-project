#include <stdio.h>
#include <iostream>
#include <string>

#include "raylib.h"
#include "raygui.h"
#include "rlgl.h"

#include "canvas2d.h"
#include "rayvideocap.h"
#include "raygraph.h"

bool DONE = false;

int main(int argc, char** argv)
{	
	int screenWidth = 640;
	int screenHeight = 480;

	InitWindow(screenWidth, screenHeight, "raylib opencv test2: camera");
	SetTargetFPS(60);

	SetTraceLogLevel(LOG_ALL);
	{
		rlVideo video( {320,240}, {10,10} );
		rlGrpah graph( {320,210}, {10,260} );

		graph.zone = { (float)screenWidth/2.0f - 10, (float)screenHeight/2.0f - 10 ,20,20};

		bool activate = false;
		bool activate_limit_box_activate = false;
		char activate_limit_in[100] = "0";
		float activate_limit_sec = 0;

		while (!WindowShouldClose()) {
			video.update();
			if ( video.newframe ) graph.calculate( video.getImage() );
			graph.update();

			BeginDrawing();
			ClearBackground(RAYWHITE);

			video.draw();
			graph.draw();

			// this is app fps
			DrawFPS( 15, 15 );

			// draw UI frame
			DrawLine( 5,5,5,screenHeight-5, BLACK );
			DrawLine( 5,5,screenWidth-5,5, BLACK );
			DrawLine( screenWidth-5,screenHeight-5,screenWidth-5,5, BLACK );
			DrawLine( screenWidth-5,screenHeight-5,5,screenHeight-5, BLACK );

			DrawLine( 335,5,335,screenHeight-5, BLACK );
			DrawLine( 5,255,335,255, BLACK );

			// draw control
			{
				char buff[50];

				sprintf(buff, "DESIRED FRAMERATE : %3d", (int)video.fps);
				DrawText( buff, 350, 15, 10, GRAY);
			
				video.fps = GuiSliderBar( {350+160,15,90,10}, NULL, NULL, 
					video.fps,
					video.fps_min,
					video.fps_max
				);
			}

			if (GuiButton( { 340+5, (float)screenHeight-10-30, 290-10, 20 }, graph.activate ? "STOP":"START")) {
				if (!graph.activate) graph.clearList();
			
				graph.activate_time = GetTime();
				graph.activate = !graph.activate;
			}

			if ( activate_limit_sec != 0 && GetTime() - graph.activate_time >= activate_limit_sec ) {
				graph.activate = false;
				graph.activate_time = GetTime();
			}
			
			DrawText( ("INTERVAL : "+
				std::to_string(1.0/video.fps)).c_str(), 350, 30, 10, GRAY);
			DrawText( ("ACTUAL FRAMRATE "+
				std::to_string(video.actual_fps)).c_str(), 350, 45, 10, BLUE);
			DrawText( "TIME LIMIT : ", 350, 60, 10, GRAY);
			if (GuiTextBox( {350+160,60,40,10}, activate_limit_in, 20, activate_limit_box_activate ) && !activate) {
				activate_limit_box_activate = !activate_limit_box_activate;

				activate_limit_sec = (float)std::atof( activate_limit_in );

				// TraceLog(LOG_INFO,"set time limit to : %f", activate_limit_sec);
			}

			EndDrawing();
		}

		DONE = true;
	}

	CloseWindow();

	return 0;
}
