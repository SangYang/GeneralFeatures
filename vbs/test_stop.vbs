' ==============================================================
' This is a Daemon's Stop by vbscript.
' Stop *.exe running In strExeNameSet.
' ==============================================================
Option Explicit 

Dim strFilePath, strExeNameSet
strFilePath = "test_start.vbs"
strExeNameSet = Array( _
	"vbs1.exe", _
	"vbs 2.exe")
	
Dim vtNowDate
vtNowDate = Now()	

Dim clsImport
Set clsImport = New Import
clsImport.Function strFilePath, "GetFileName"
clsImport.Class strFilePath, "Process"
Set clsImport = Nothing
	
Dim clsProcess
Set clsProcess = New Process

clsProcess.StopPrevious "wscript.exe", vtNowDate

Dim strExeName
For Each strExeName In strExeNameSet
	clsProcess.Stop strExeName
Next

Dim strTipInfo
For Each strExeName In strExeNameSet
	strTipInfo = strTipInfo + Chr(13) + strExeName + Chr(13)
Next
MsgBox strTipInfo, 0, "Stop *.EXE Running"

Set clsProcess = Nothing

'-----------------------------------------------------------
Class Import
	Private objFileSys

	Private Sub Class_Initialize 
		Set objFileSys = CreateObject("Scripting.FileSystemObject")
	End Sub 
	
	Private Sub Class_Terminate()
		Set objFileSys = Nothing
	End Sub	
	
	Private Function GetImportStr(objStream, strName, strType)
		With objStream
			Dim strLine, flagAdd
			flagAdd = -1
			Do While objStream.AtEndOfStream <> TRUE			
				strLine = .ReadLine()
				If 0 < InStr(LCase(strLine), LCase(strType)) _
					And 0 < InStr(LCase(strLine), LCase(strName)) Then
					flagAdd = 1
				ElseIf 0 < InStr(LCase(strLine), LCase("End")) _
					And 0 < InStr(LCase(strLine), LCase(strType)) Then
					flagAdd = 0
				End If
				If 1 = flagAdd Then
					GetImportStr = GetImportStr + strLine + Chr(13)	
				ElseIf 0 = flagAdd Then
					GetImportStr = GetImportStr + strLine + Chr(13)	
					Exit Do
				End If		
			Loop
		End With
	End Function	
	
	Public Sub [Function](strFilePath, strFuncName)
		Dim objStream, strFunc
		Set objStream = objFileSys.OpenTextFile(strFilePath, 1)
		strFunc = GetImportStr(objStream, strFuncName, "Function")
		objStream.Close()
		Set objStream = Nothing			
		ExecuteGlobal strFunc
	End Sub
	
	Public Sub [Class](strFilePath, strClassName)
		Dim objStream, strClass
		Set objStream = objFileSys.OpenTextFile(strFilePath, 1)
		strClass = GetImportStr(objStream, strClassName, "Class")
		objStream.Close()
		Set objStream = Nothing
		ExecuteGlobal strClass
	End Sub	
End Class
