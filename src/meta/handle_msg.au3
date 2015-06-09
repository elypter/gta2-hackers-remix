#include "cmds/singleplayer.au3"
#include "cmds/splitscreen.au3"

Func handle_msg($msg)

	; The first message from the menu is always the config path
	If Not $global_config_path Then
		$global_config_path = $msg
		If Not FileExists($global_config_path) Then
			Exit Msgbox(16, "G2HR", "ERROR: Config path doesn't" _
				& " exist: " & @CRLF & $global_config_path & @CRLF _
				& "This shouldn't have happened, please report a" _
				& " bug at:" & @CRLF &  "http://git.io/g2hr-bugs")
		Endif
		re("LOOKS VALID :)")
	Endif
	
	
	; Handle incoming commands. Format:
	; 	COMMAND_NAME [PARAMETER1 [PARAMETER2] ... ]
	Local $cmd = StringSplit($msg," ",2)
	Switch $cmd[0]
		Case "CLEANUP"
			Return False
		Case "SINGLEPLAYER"
			cmd_singleplayer($cmd)
			Sleep(500)
			re("HIDE GET READY SCREEN")
		Case "SCREENLAYOUT"
			Local $geo[4] = [$cmd[2], $cmd[3], $cmd[4], $cmd[5]]
			$global_game_screen_layouts[$cmd[1]] = $geo
		Case "SPLITSCREEN"
			cmd_splitscreen($cmd)
			Sleep(500)
			re("HIDE GET READY SCREEN")
		Case "QUIT"
			ProcessClose($global_game_process_ids[$cmd[1]])
	EndSwitch
	Return True
Endfunc
