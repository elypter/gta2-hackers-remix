#include <WinAPI.au3>
#include <Constants.au3>

; Constants (see also: common.h)
Global Const $GTA2_PLAYER_COUNT = 6
Global Const $GTA2_LOBBY_CTRL_START = 1021
Global Const $GTA2_LOBBY_CTRL_LIST  = 1024
Global Const $GTA2_GAME_WINDOW_DESC = "[TITLE:GTA2;CLASS:WinMain]"
Global Const $WINE = is_running_in_wine()
Global Const $HWND_SDL = WinGetHandle("[TITLE:G2HR;CLASS:SDL_app]")


; Global variables
Global $global_sock


; This gets generated by SDL2 in the menu and has a trailing slash.
; The menu sends this as soon, as the meta component connects.
Global $global_config_path = NULL

Global $global_game_instances_open = 0
Global $global_game_process_ids[$GTA2_PLAYER_COUNT]
Global $global_game_screen_layouts[$GTA2_PLAYER_COUNT]

; Initialize Arrayss
For $i=0 To $GTA2_PLAYER_COUNT-1
	$global_game_process_ids[$i] = 0
Next

; Zero-terminated string reply to the server
Func re($message)
	TCPSend($global_sock, $message & Chr(0))
Endfunc


Func send_pid_table()
	Local $str = "PID_TABLE"
	For $i=0 To $GTA2_PLAYER_COUNT - 1
		$str &= " " & $global_game_process_ids[$i]
	Next
	re($str)
Endfunc


; Return values:
; 		$ret[0]: count of hwnds
; 		$ret[1]: first hwnd
; 		...
Func get_all_hwnds_from_pid($pid, $desc = "")
	Local $list = WinList($desc)
	Local $ret[1]
	$ret[0] = 0
	
	For $i = 1 To $list[0][0]
	
		Local $hwnd = $list[$i][1]
		
		If WinGetProcess($hwnd) == $pid Then
			_ArrayAdd($ret, $hwnd)
			$ret[0] += 1
		Endif
	Next
	
	Return $ret
Endfunc

Func find_hwnd_with_control($hwnds, $ctrl_id)
	For $i=1 To $hwnds[0]
		If ControlCommand($hwnds[$i], "", $ctrl_id, "IsEnabled") Then
			Return $hwnds[$i]
		Endif
	Next
	
	Return Null
Endfunc

Func wait_for_hwnd_with_control($pid, $ctrl_id)
	While True
		Local $hwnd = find_hwnd_with_control( _
			get_all_hwnds_from_pid($pid), $ctrl_id)
		If $hwnd Then _
			Return $hwnd
		
		Sleep(100)
	Wend
Endfunc

Func wait_for_hwnd_with_desc($pid, $desc)
	While True
		Local $hwnds = get_all_hwnds_from_pid($pid, $desc)
		If $hwnds[0] > 0 Then _
			Return $hwnds[1]
	
		Sleep(100)
	Wend
Endfunc

; Returns the single hwnd id that is left
Func wait_until_only_one_hwnd_left($pid)
	While True
		Local $hwnds = get_all_hwnds_from_pid($pid)
		
		If IsArray($hwnds) And $hwnds[0] == 1 Then _
			Return $hwnds[1]
		
		
		Sleep(100)
	Wend
Endfunc

Func wait_for_listview_entry_count($hwnd, $ctrl_id, $count)
	While True
		Local $current = ControlListView($hwnd, "", $ctrl_id, _
			"GetItemCount")
		
		If $current == $count Then _
			Return
		
		Sleep(100)
	Wend
Endfunc

Func move_until_it_works($hwnd, $geo)
	; WinGetPos doesn't work here (because the window isn't visible
	; yet?). However, these functions do work. We measure the window-
	; border to put it off-screen.
	Local $current_width = _WinAPI_GetWindowWidth($hwnd)
	Local $current_height = _WinAPI_GetWindowHeight($hwnd)
	
	; left, bottom, right
	Local $border_lbr = ($current_width - $geo[2]) / 2
	Local $border_top = $current_height - $geo[3] - $border_lbr
	
	Local $x = $geo[0] - $border_lbr
	Local $y = $geo[1] - $border_top
	
	; ConsoleWrite("x: " & $x & ", y: " & $y & @CRLF)
	
	; FIXME: this doesn't work as expected in wine (top offset ignored?)
	_WinAPI_SetWindowPos($hwnd, $HWND_TOP, $x, $y, $current_width, _
		$current_height, $SWP_NOSIZE)
Endfunc

Func regwrite_if_empty($keyname, $valuename, $type, $value)
	RegRead($keyname, $valuename)
	If @Error Then _
		RegWrite($keyname, $valuename, $type, $value)
EndFunc

Func is_running_in_wine()
	RegRead("HKEY_CURRENT_USER\Software\Wine", "")
	If @error > 0 Then Return False
	Return True
EndFunc


