#pragma execution_character_set("utf-8")

#pragma warning(disable : 28251)


// UTF-8 encoding: あいうえお


#include "Tab1.h"
#include "Tab2.h"
#include "pawnline.h"
#include "utility.h"
#include <wx/wx.h>
#include <wx/accel.h>
#include <wx/notebook.h>
#include <wx/filedlg.h>

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

// 保存通知用の簡易パネルクラス
class SaveNotificationPanel : public wxPanel {
public:
    SaveNotificationPanel(wxWindow* parent, const wxString& message)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(190, 40),
            wxBORDER_SIMPLE | wxSTAY_ON_TOP), timer(this)
    {
        SetBackgroundColour(wxColour(150, 200, 255, 128)); // 明るい青

        m_label = new wxStaticText(this, wxID_ANY, message, wxPoint(10, 10));
        m_label->SetForegroundColour(*wxBLACK);
        m_label->SetFont(wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, ""));
        m_label->SetForegroundColour(wxColour(255, 255, 255)); // 白
        Hide();

        // タイマーが止まったら非表示にする
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
            Hide();
            }, timer.GetId());
    }

    void ShowNotification(int milliseconds = 3000) {
        Show();
        Raise();  // 最前面に
        timer.StartOnce(milliseconds);
    }

    void SetMessage(const wxString& message) {
        m_label->SetLabel(message);
    }

private:
    wxStaticText* m_label;
    wxTimer timer;
};


class MainFrame : public wxFrame {
public:
    MainFrame()
        : wxFrame(nullptr, wxID_ANY, "キャラクター管理アプリ", wxDefaultPosition, wxSize(630, 800)),
        notebook(nullptr), tab1Panel(nullptr), tab2Panel(nullptr), pawnLinePanel(nullptr)  // メンバ変数を初期化
    {
        // wxNotebook（タブ）を作成
        notebook = new wxNotebook(this, wxID_ANY);

        // Tab1Panel のインスタンスを先に作成
        tab1Panel = new Tab1Panel(notebook);
        notebook->AddPage(tab1Panel, "データ表示");

        // Tab2Panel を作成し、 Tab1Panel を渡す
        tab2Panel = new Tab2Panel(notebook, notebook, tab1Panel);
        notebook->AddPage(tab2Panel, "能力作成");

        pawnLinePanel = new DynamicPawnPanel(notebook);
        notebook->AddPage(pawnLinePanel, "上級戦闘(GM向け)");


        // メニューバーを作成
        wxMenuBar* menuBar = new wxMenuBar();
        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(wxID_OPEN, "ファイルを読み込む(Ctrl+F)");
        fileMenu->Append(wxID_SAVEAS, "名前を付けてファイルを保存(Ctrl+shift+S)");
        fileMenu->Append(wxID_SAVE, "ファイルを上書き保存(Ctrl+S)");
        fileMenu->Append(wxID_EXIT, "終了");
        menuBar->Append(fileMenu, "ファイル");
        SetMenuBar(menuBar);

        // イベントハンドラを設定
        Bind(wxEVT_MENU, &MainFrame::OnOpenFile, this, wxID_OPEN);
        Bind(wxEVT_MENU, &MainFrame::OnSaveFile, this, wxID_SAVEAS);
        Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

        // ショートカットキー設定
        wxAcceleratorEntry entries[3];
        entries[0].Set(wxACCEL_CTRL, (int)'F', wxID_OPEN);
        entries[1].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
        entries[2].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'S', wxID_SAVEAS);
        wxAcceleratorTable accel(3, entries);
        SetAcceleratorTable(accel);

        // 通知パネルの作成と初期非表示
        notificationPanel = new SaveNotificationPanel(this, "保存完了");
        notificationPanel->SetPosition(wxPoint(GetClientSize().GetWidth() - 220, 10));
        notificationPanel->Hide();


    }


private:
    wxNotebook* notebook;   // wxNotebook をメンバ変数として管理
    Tab1Panel* tab1Panel;   // Tab1Panel へのポインタ
    Tab2Panel* tab2Panel;   // Tab2Panel へのポインタ
    DynamicPawnPanel* pawnLinePanel; // pawnLinePanel へのポインタ
    wxString currentFilePath; //  ファイルパスを保存する変数

    SaveNotificationPanel* notificationPanel; // 通知用パネル

    // JSONファイルを開く
    void OnOpenFile(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "JSONファイルを開く", "", "", "JSONファイル (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = openFileDialog.GetPath();
        std::ifstream file(filePath.ToStdString());
        if (!file) {
            wxMessageBox("ファイルを開けませんでした", "エラー", wxOK | wxICON_ERROR);
            return;
        }

        json jsonData;
        file >> jsonData;
        file.close();

        // Tab1Panelのインスタンスを取得し、LoadFromJsonを呼び出してデータをセット
        if (tab1Panel) {
            tab1Panel->LoadFromJson(filePath);  // jsonDataを渡してセット
        }
        else {
            wxMessageBox("Tab1Panelのインスタンスが見つかりませんでした", "エラー", wxOK | wxICON_ERROR);
        }

        currentFilePath = filePath; //  新しいファイルパスを保存
        notificationPanel->SetMessage("ファイルの読み込み完了");
        notificationPanel->ShowNotification();
        //wxMessageBox("ファイルを読み込みました:\n" + wxString(jsonData.dump(4)), "成功", wxOK | wxICON_INFORMATION);
    }

    // JSONファイルを保存
    void OnSaveFile(wxCommandEvent& event) {
        wxFileDialog saveFileDialog(this, "名前を付けて保存", "", "新規キャラクター", "JSONファイル (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = saveFileDialog.GetPath();
        tab1Panel->SaveToJson(filePath);

        // 保存が成功したかを判定
        if (!tab1Panel->SaveToJson(filePath)) {
            wxMessageBox("ファイルの保存に失敗しました", "エラー", wxOK | wxICON_ERROR);
            return;
            
        }

        currentFilePath = filePath; //  新しいファイルパスを保存
        notificationPanel->SetMessage("名前を付けて保存完了");
        notificationPanel->ShowNotification();
    }


    void OnSave(wxCommandEvent& event) {
        if (currentFilePath.IsEmpty()) {
            // ファイルパスが未設定なら「名前を付けて保存」を実行
            OnSaveFile(event);
        }
        else {
            
            if (!tab1Panel->SaveToJson(currentFilePath)) {
                wxMessageBox("ファイルの保存に失敗しました", "エラー", wxOK | wxICON_ERROR);
            }
            // 既存のファイルに上書き保存
            notificationPanel->SetMessage("上書き保存完了");
            notificationPanel->ShowNotification();
        }


    }

    // アプリを終了
    void OnExit(wxCommandEvent& event) {
        Close(true);
    }
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MainFrame* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
