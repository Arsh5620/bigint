#include <iostream>
#include <windows.h>
#include <malloc.h>
#include <sstream>
#include "LibBigInt.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND textBox1, textBox2, label1, label2,
button1, comboLabel1, combo1, output1, output2;
#define MAX_TEXT_EDIT_LENGTH 65536
#define	IDC_MY_BUTTON 0x8001
#define IDC_OUTPUT_LABEL 0x8002
HINSTANCE gHInstance;
#define FILTER_WIDTH 42
LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_COMMAND: {
			switch (HIWORD(wParam)) {
				//hi word of wParam is control defined notification code and the lo word 
				//is control identifier and the lparam is handle to control window
				case BN_CLICKED: {
					if (IDC_MY_BUTTON == LOWORD(wParam)) {
						LRESULT length = ::GetWindowTextLength(textBox1);
						LRESULT length2 = ::SendMessage(textBox2, WM_GETTEXTLENGTH, NULL, NULL);
						//Getting the length of text in the edit control
						char *textBuffer = (char*)malloc(length + 1);
						char *textBuffer2 = (char*)malloc(length2 + 1);
						ZeroMemory(textBuffer, length + 1);
						ZeroMemory(textBuffer2, length2 + 1);
						//Now preparing the buffer to get the text from the edit
						::GetWindowText(textBox1, textBuffer, length + 1);
						::GetWindowText(textBox2, textBuffer2, length2 + 1);
						BOOLEAN b1 = LibBigInt::isNumeric(textBuffer), b2 = LibBigInt::isNumeric(textBuffer2);
						if (!b1 && !b2) {
							::MessageBox(window, "Both of the fields contains non numeric characters.", "Error", NULL);
						}
						else if (!b1) {
							::MessageBox(window, "The first field contains non numeric characters.", "Error", NULL);
						}
						else if (!b2) {
							::MessageBox(window, "The second field contains non numeric characters.", "Error", NULL);
						}
						else
						{
							//Now we can be sure that both of the text fields contains only numbers
							LRESULT selected = ::SendMessage(combo1, CB_GETCURSEL, NULL, NULL);
							if (selected != CB_ERR) {
								//And now we are sure that every field has a correct value
								//46 is the maximum length of the split, 46*12 = 552
								try
								{
									LibBigInt a1(textBuffer, length);
									LibBigInt a2(textBuffer2, length2);
									LibBigInt *a3 = 0;
									std::stringstream stream;
									switch (selected) {
										case 0:a3 = a1 + &a2;
											stream << "Addition :\r\n";
											break;
										case 1:a3 = a1 - &a2;
											stream << "Substraction :\r\n";
											break;
										case 2: a3 = a1* &a2;
											stream << "Multiplication :\r\n";
											break;
										case 3:a3 = a1 / &a2;
											stream << "Division :\r\n";
											break;
										default: a3 = NULL;
									}
									::free(textBuffer);
									::free(textBuffer2);
									if (a3 != 0) {
										char *number = LibBigInt::getOver(a3->getData().data, '0');
										int stL = ::strlen(number);
										if (selected == 3)
											stream << "The Quotient is" << (stL ? (a3->getData().isNegative ? " : Negative" : " : Positive") : "") << " : " << (stL ? number : "Zero")<< std::endl<<"\r\nRemainder : "<<(a3->getAddOps().size?a3->getAddOps().memory:"Zero");
										else
											stream << "The result is" << (stL ? (a3->getData().isNegative ? " : Negative" : " : Positive") : "") << " : " << (stL ? number : "Zero");
										::SendMessage(output2, WM_SETTEXT, NULL, (LPARAM)stream.str().c_str());
									}
									else {
										::SendMessage(output2, WM_SETTEXT, NULL, (LPARAM)"An error occured, try again!");
									}
									a3->~LibBigInt();
									RedrawWindow(window, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASE);
								}
								catch (char *string) {
									::MessageBox(NULL, string, "An error occured", MB_ICONERROR);
								}
							}
							else {
								::MessageBox(window, "None of the fields are selected for the operation", "Error", NULL);
							}
						}
						//::SendMessage(textBox1,WM_GETTEXT,(WPARAM)length+1,(LPARAM)textBuffer);
					}
					break;
				}
			}
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			//Just so that it won't be handled by default window procedure
			return(0);
		}
		case WM_CREATE: {
			//::MessageBox(NULL, "The program is still in beta phase and may contains some error or bugs, use at your own risk!", "Warning!", MB_ICONINFORMATION);
			HFONT font = ::CreateFont(18, NULL, NULL, NULL, 200, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "SEGOE UI LIGHT");
			label1 = ::CreateWindow("static", "First variable: ", WS_CHILD | WS_VISIBLE, 2, 4, 120, 22, window, NULL, gHInstance, NULL);
			label2 = ::CreateWindow("static", "Second variable: ", WS_CHILD | WS_VISIBLE, 2, 30, 120, 22, window, NULL, gHInstance, NULL);
			textBox1 = ::CreateWindow("edit", "1234567890", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 124, 2, 140, 22, window, NULL, gHInstance, NULL);
			textBox2 = ::CreateWindow("edit", "1234567890", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 124, 28, 140, 22, window, NULL, gHInstance, NULL);
			button1 = ::CreateWindow("button", "Calculate", WS_CHILD | WS_VISIBLE, 124, 320, 140, 26, window, (HMENU)IDC_MY_BUTTON, gHInstance, NULL);
			comboLabel1 = ::CreateWindow("static", "Operator", WS_CHILD | WS_VISIBLE, 2, 58, 120, 22, window, NULL, gHInstance, NULL);
			combo1 = ::CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST, 124, 56, 140, 30, window, NULL, gHInstance, NULL);
			output1 = ::CreateWindow("static", "Output: ", WS_CHILD | WS_VISIBLE, 2, 84, 120, 22, window, NULL, gHInstance, NULL);
			output2 = :://CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 2, 108, 280, 200, window, (HMENU)IDC_OUTPUT_LABEL, gHInstance, NULL);
				CreateWindowEx(
					0, "EDIT",   // predefined class 
					NULL,         // no window title 
					WS_CHILD | WS_VISIBLE | WS_VSCROLL |
					ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
					2, 108, 280, 200,   // set size in WM_SIZE message 
					window,         // parent window 
					(HMENU)IDC_OUTPUT_LABEL,   // edit control ID 
					gHInstance,
					NULL);//comboLabel1 messages
			//HWND myCustom = ::CreateWindow(MY_CUSTOM_VIEW, "mywindow", WS_CHILD | WS_VISIBLE, 20, 20, 400, 320, window, NULL, gHInstance, NULL);
			::SendMessage(comboLabel1, WM_SETFONT, (WPARAM)font, NULL);
			//output1 messages
			::SendMessage(output1, WM_SETFONT, (WPARAM)font, NULL);
			//output2 messages
			::SendMessage(output2, WM_SETFONT, (WPARAM)font, NULL);
			//combo box messages
			::SendMessage(combo1, CB_ADDSTRING, NULL, (LPARAM)"+");
			::SendMessage(combo1, CB_ADDSTRING, NULL, (LPARAM)"-");
			::SendMessage(combo1, CB_ADDSTRING, NULL, (LPARAM)"*");
			::SendMessage(combo1, CB_ADDSTRING, NULL, (LPARAM)"/");
			::SendMessage(combo1, WM_SETFONT, (WPARAM)font, TRUE);
			::SendMessage(combo1, CB_SETCURSEL, 0, NULL);
			//button1 messages
			::SendMessage(button1, WM_SETFONT, (WPARAM)font, TRUE);
			//textbox1 messages
			::SendMessage(textBox1, WM_SETFONT, (WPARAM)font, TRUE);
			::SendMessage(textBox1, EM_SETLIMITTEXT, MAX_TEXT_EDIT_LENGTH, 0);
			//textbox2 messages
			::SendMessage(textBox2, WM_SETFONT, (WPARAM)font, TRUE);
			::SendMessage(textBox2, EM_SETLIMITTEXT, MAX_TEXT_EDIT_LENGTH, 0);
			//label1 messages
			::SendMessage(label1, WM_SETFONT, (WPARAM)(font), TRUE);
			::SendMessage(label2, WM_SETFONT, (WPARAM)(font), TRUE);
			break;
		}
	}
	return(::DefWindowProc(window, message, wParam, lParam));
}
static char title[] = "Arshdeep Singh - Creative Thinking";
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR lpCmdLine, int nCmdShow) {
	//MyCustomView customView(hInstance);
	gHInstance = hInstance;
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = ::WndProc;
	wcex.lpszMenuName = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = "expreval";
	//So now we are done with creating the class
	//Now we need to register the class before we can actually use it
	if (!::RegisterClassEx(&wcex)) {
		::MessageBox(NULL, "An error occured while registering the main class for the program", "Critical error", MB_ICONERROR);
		return(-1);
	}
	HWND hWnd = ::CreateWindow("expreval", title, WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 300, 400, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		::MessageBox(NULL, "An error occured while creating the window from a registered class", "Critical error", MB_ICONERROR);
		return(-1);
	}
	//Now we are done with registering and creating an instance of the window
	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);
	//Now we are done with showing the window, but it exits at an instant because the program exits as soon as it has shown the program
	//Now we are to create a message loop
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return(msg.wParam);
}