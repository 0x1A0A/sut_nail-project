#include <stdio.h>
#include <iostream>
#include <string>

#include "raylib.h"
#include "raygui.h"
#include "rlgl.h"

#include "canvas2d.h"
#include "video/rayvideocap.h"
#include "graph/raygraph.h"
#include "savecsv.h"

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

		const float zonesize = 20;

		graph.zone = {
			((float)screenWidth - zonesize)/2,
			((float)screenHeight - zonesize)/2,
			zonesize,zonesize
		};

		bool activate = false;
		bool activate_limit_box_activate = false;
		char activate_limit_in[100] = "0";
		float activate_limit_sec = 0;
		int val = 20;
		bool spinedit = false;
		char filename[256] = "SAVE";
		bool filenameEdit = false;
		while (!WindowShouldClose()) {
			video.update();
			if ( video.newframe ) graph.calculate( video.getImage() );
			graph.update();

			BeginDrawing();
			ClearBackground(RAYWHITE);

			video.draw();
			graph.draw();

			DrawRectangleLines( 
				video.getPos().x + graph.zone.x - video.getSize().x/2, 
				video.getPos().y + graph.zone.y - video.getSize().y/2,
				graph.zone.width, graph.zone.height, WHITE
			);

			graph.autoscroll = GuiCheckBox( 
				{ graph.getPos().x + 10, graph.getPos().y, 20, 10 },
				"auto scroll", graph.autoscroll
			);

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
			DrawText(("Zone size : "+
				std::to_string(val)).c_str(), 350, 75, 10, GRAY);
			val = GuiSlider( {350 + 160, 75, 60, 10}, "","", val,10,50 );

			graph.zone = {
				((float)screenWidth - val)/2,
				((float)screenHeight - val)/2,
				(float)val,(float)val
			};

			if (GuiTextBox( { 340+5, (float)screenHeight-10-45-10, 280-50, 20 },filename,10, filenameEdit)) {
				filenameEdit = !filenameEdit;
			}

			if (GuiButton({ 340+5+280-40, (float)screenHeight-10-45-10, 40, 20 }, "SAVE")) {
				saveListToCSV(graph.getList(), filename);
			}

			if (GuiButton( { 340+5, (float)screenHeight-10-30, 290-10, 20 }, graph.activate ? "STOP":"START")) {
				if (!graph.activate) graph.clearList();
			
				graph.activate_time = GetTime();
				graph.activate = !graph.activate;
			}

			EndDrawing();
		}

	}

	CloseWindow();

	return 0;
}
