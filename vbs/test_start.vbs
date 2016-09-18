' ==============================================================
' This is a Daemon's Start by vbscript.
' Start *.exe In strExePathSet keep running all the time.
' ==============================================================
Option Explicit 

Dim strExePathSet
strExePathSet = Array( _
	"1a\vbs1.exe", _
	"2 b\vbs 2.exe")

Dim vtNowDate
vtNowDate = Now()
	
Dim clsProcess
Set clsProcess = New Process

clsProcess.StopPrevious "wscript.exe", vtNowDate

Dim strExeName, strExePath
For Each strExePath In strExePathSet
	strExeName = GetFileName(strExePath)
	clsProcess.Stop strExeName
	WScript.Sleep 1000
	clsProcess.Start strExePath
Next

Dim strTipInfo
For Each strExePath In strExePathSet
	strTipInfo = strTipInfo + Chr(13) + strExePath + Chr(13)
Next
MsgBox strTipInfo, 0, "Start Daemon *.EXE"

Dim exist
Do While TRUE
	For Each strExePath In strExePathSet
		strExeName = GetFileName(strExePath)
		exist = clsProcess.Exist(strExeName)
		If FALSE = exist Then
			WScript.Sleep 1000		
			clsProcess.Start strExePath
		End If	
	Next
	WScript.Sleep 5000
Loop

Set clsProcess = Nothing

'-----------------------------------------------------------
Function GetFileName(strPath)
	Dim arrPath
	arrPath = split(strPath, "\")
	GetFileName = arrPath(ubound(arrPath))
End Function	

Function GetFileDir(strPath)
	Dim strSubPath
	strSubPath = InstrRev(strPath, "\")
	GetFileDir = Left(strPath, strSubPath)
End Function

Class Process
	Private objWshShell
	Private objWMIService
	Private objDatetime
	Private objFileSys

	Private Sub Class_Initialize 
		Set objWshShell = WScript.CreateObject("WScript.Shell")
		Set objWMIService = GetObject("winmgmts:\\.\root\cimv2")
		Set objDatetime = CreateObject("WbemScripting.SWbemDateTime")	
		Set objFileSys = CreateObject("Scripting.FileSystemObject")		
	End Sub 
	
	Private Sub Class_Terminate()
		Set objWshShell = Nothing
		Set objWMIService = Nothing
		Set objDatetime = Nothing
		Set objFileSys = Nothing
	End Sub	
	
	Public Sub Start(strPath)
		Dim strCurrentDir, strDir, strName, strNewPath
		strCurrentDir = objWshShell.CurrentDirectory
		strDir = GetFileDir(strPath)
		objWshShell.CurrentDirectory = strDir	
		
		strName = GetFileName(strPath)
		strNewPath = objFileSys.GetFolder(".").Path	+ "\" + strName	
		objWshShell.Run Chr(34) & strNewPath & Chr(34),1,FALSE    
		objWshShell.CurrentDirectory = strCurrentDir
	End Sub

	Public Sub [Stop](strName)
		Dim colProcessSet, objProcess
		Set colProcessSet = objWMIService.ExecQuery _
			("SELECT * FROM Win32_Process WHERE Name='" & strName & "'")
		For Each objProcess In colProcessSet
			If objProcess.Name = strName Then
				objWshShell.Run "TASKKILL /F /IM " & Chr(34) & strName & Chr(34), 0
				Exit Sub
			End If
		Next	
	End Sub

	Public Function Exist(strName)
		Dim colProcessSet, objProcess
		Set colProcessSet = objWMIService.ExecQuery _
			("SELECT * FROM Win32_Process WHERE Name='" & strName & "'")	
		For Each objProcess In colProcessSet
			If objProcess.Name = strName Then
				Exist = TRUE
				Exit Function
			End If
		Next	
		Exist = FALSE
	End Function

	Private Function DatetimeToDate(cimDatetime)
		objDatetime.Value = cimDatetime
		DatetimeToDate = objDatetime.GetVarDate(True)
	End Function

	Private Function DateSecondDiff(vtPrevDate, vtAfterDate)
		DateSecondDiff = DateDiff("s", vtPrevDate, vtAfterDate)
	End Function

	Public Sub StopPrevious(strName, vtDate)
		Dim colProcessSet, objProcess, processSecDiff
		Set colProcessSet = objWMIService.ExecQuery _
			("SELECT * FROM Win32_Process WHERE Name='" & strName & "'")	
		For Each objProcess In colProcessSet
			If objProcess.Name = strName Then
				processSecDiff = DateSecondDiff(vtDate, DatetimeToDate(objProcess.CreationDate))
				If 0 > processSecDiff Then
					objWshShell.Run "TASKKILL /F /PID " & objProcess.Handle, 0
				End If		
			End If
		Next	
	End Sub
End Class
