# Microsoft Developer Studio Project File - Name="AvejLite_Launcher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AvejLite_Launcher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AvejLite_Launcher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvejLite_Launcher.mak" CFG="AvejLite_Launcher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvejLite_Launcher - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AvejLite_Launcher - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AvejLite_Launcher - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".." /I "../external/wiz" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /Zm500 /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"./bin/AvejLite_Launcher.exe" /libpath:"../external/wiz"

!ELSEIF  "$(CFG)" == "AvejLite_Launcher - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "avej_lite/adaptation" /I "util" /I "external/wiz" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_MSVC" /D DEVICE_RES=DEVICE_RES_800x480 /FR /YX /FD /GZ /Zm500 /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"./bin/AvejLite_Launcher_d.exe" /libpath:"external/wiz"
# SUBTRACT LINK32 /profile

!ENDIF 

# Begin Target

# Name "AvejLite_Launcher - Win32 Release"
# Name "AvejLite_Launcher - Win32 Debug"
# Begin Group "avej_lite"

# PROP Default_Filter ""
# Begin Group "adaptation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\avej_lite\adaptation\avej_app_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\avej_gfx_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\avej_main.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\avej_util_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\gfx3d_config.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\gfx3d_opengl.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\gfx3d_opengl_osal.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\adaptation\gfx3d_type.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\avej_lite\avej_app.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_base_type.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_config.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_gfx.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_pixel_format.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_sound_mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_sound_mixer.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_util.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_util_iu.h
# End Source File
# Begin Source File

SOURCE=.\avej_lite\avej_util_tga_decoder.cpp
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\util\util_convert_to_ucs.cpp
# End Source File
# Begin Source File

SOURCE=.\util\util_convert_to_ucs.h
# End Source File
# Begin Source File

SOURCE=.\util\util_convert_to_ucs_table.cpp
# End Source File
# Begin Source File

SOURCE=.\util\util_render_text.cpp
# End Source File
# Begin Source File

SOURCE=.\util\util_render_text.h
# End Source File
# End Group
# Begin Group "src_PlutoidLite"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_device3d.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_device3d.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_fvf.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_fvf.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_sample.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_scene.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_scene.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_type.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\gfx3d_util.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_albireo.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_albireo.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_caina_crena.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_caina_crena.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_oracle.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_oracle.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_the_absolute.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_actor_the_absolute.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_premotion.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_premotion.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation_base.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation_base.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation_dream.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_presentation_kano.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_world.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_base_world.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_app_base.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_console.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_console.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_player.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_simple_tile_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_class_simple_tile_map.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_config.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_config.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_data_communication.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_data_communication.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_res.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_util.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_visible.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\pd_visible_actor.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\res_avej_tex1.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\res_avej_tex2.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\util_fos_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\util_fos_map.h
# End Source File
# Begin Source File

SOURCE=.\src_PlutoidLite\util_tile_map_base.h
# End Source File
# End Group
# Begin Group "src_ShitFighter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src_ShFgtr\res_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\res_shit_img.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\res_story.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_config.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_gfx.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_main.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_main.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_map.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj_enemy.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj_enemy.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj_friend.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_obj_friend.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_res.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_res.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_ending_bad.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_ending_happy.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_game_play.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_menu_about.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_menu_option.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_menu_story.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_state_title.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_sys_desc.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_sys_desc.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr\sf_util.h
# End Source File
# End Group
# Begin Group "src_ManoEri"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src_ManoEri\me_fos_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_fos_map.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_main.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_main.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_player.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_player.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_res.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_tile_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_tile_map.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_type.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\me_visible_map.h
# End Source File
# Begin Source File

SOURCE=.\src_ManoEri\res_img0.cpp
# End Source File
# End Group
# Begin Group "src_LoreBase"

# PROP Default_Filter ""
# Begin Group "util_LoreBase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src_LoreBase\_USmFont12x12.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\_USmFont12x12.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmRandom.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmScript.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmSet.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmSola.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmStream.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USmStream.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bin\lore_ep1.cm2
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UConfig.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UConsole.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UExtern.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UGameOption.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UGameOption.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UKeyBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UKeyBuffer.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UMain.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UMain.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UMap.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UMap.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcEnemy.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcEnemy.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcNameBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcNameBase.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcParty.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcParty.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UPcPlayer.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UResString.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UResString.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UScript.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\UScript.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USelect.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USelect.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USerialize.h
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USound.cpp
# End Source File
# Begin Source File

SOURCE=.\src_LoreBase\USound.h
# End Source File
# End Group
# Begin Group "src_ShitFighter_Wave"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_config.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_gfx.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_main.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_main.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_map.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj_enemy.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj_enemy.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj_friend.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_obj_friend.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_res.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_res.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_res_map.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_res_shit_img.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_res_story.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_ending_bad.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_ending_happy.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_game_play.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_menu_about.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_menu_option.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_menu_story.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_state_title.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_sys_desc.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_sys_desc.h
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src_ShFgtr_wave\sfwv_util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\avejapp_launcher.cpp
# End Source File
# Begin Source File

SOURCE=.\avejapp_register.cpp
# End Source File
# Begin Source File

SOURCE=.\avejapp_register.h
# End Source File
# End Target
# End Project
