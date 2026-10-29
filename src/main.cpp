#pragma warning(disable : 28251)
#include "Tab1.h"
#include "Tab2.h"
#include "Tab3.h"
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

// 独自ID
enum
{
    wxID_EXPORT = wxID_HIGHEST + 1
};

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
        : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("キャラクター管理アプリ"), wxDefaultPosition, wxSize(630, 800)),
        notebook(nullptr), tab1Panel(nullptr), tab2Panel(nullptr), tab3Panel(nullptr), pawnLinePanel(nullptr)  // メンバ変数を初期化
        {

        // wxNotebook（タブ）を作成
        notebook = new wxNotebook(this, wxID_ANY);

        // Tab1Panel のインスタンスを先に作成
        tab1Panel = new Tab1Panel(notebook);
        notebook->AddPage(tab1Panel, wxString::FromUTF8("データ表示"));

        // Tab3Panel を作成する
        tab3Panel = new Tab3Panel(notebook, tab1Panel);
        tab3Panel->SetTab1Panel(tab1Panel);
        notebook->AddPage(tab3Panel, wxString::FromUTF8("セッション履歴"));

        // Tab2Panel を作成し、 Tab1Panel を渡す
        tab2Panel = new Tab2Panel(notebook, notebook, tab1Panel);
        notebook->AddPage(tab2Panel, wxString::FromUTF8("能力作成"));

        pawnLinePanel = new DynamicPawnPanel(notebook);
        notebook->AddPage(pawnLinePanel, wxString::FromUTF8("上級戦闘(GM向け)"));
        

        // メニューバーを作成
        wxMenuBar* menuBar = new wxMenuBar();
        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(wxID_OPEN, wxString::FromUTF8("ファイルを読み込む(Ctrl+F)"));
        fileMenu->Append(wxID_SAVEAS, wxString::FromUTF8("名前を付けてファイルを保存(Ctrl+shift+S)"));
        fileMenu->Append(wxID_SAVE, wxString::FromUTF8("ファイルを上書き保存(Ctrl+S)"));
        fileMenu->Append(wxID_EXPORT, wxString::FromUTF8("ココフォリアに出力(Ctrl+E)"));
        fileMenu->Append(wxID_EXIT, wxString::FromUTF8("終了"));
        menuBar->Append(fileMenu, wxString::FromUTF8("ファイル"));
        SetMenuBar(menuBar);

        // イベントハンドラを設定
        Bind(wxEVT_MENU, &MainFrame::OnOpenFile, this, wxID_OPEN);
        Bind(wxEVT_MENU, &MainFrame::OnSaveFile, this, wxID_SAVEAS);
        Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &MainFrame::OnEXPORT, this, wxID_EXPORT);
        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

        // ショートカットキー設定
        wxAcceleratorEntry entries[4];
        entries[0].Set(wxACCEL_CTRL, (int)'F', wxID_OPEN);
        entries[1].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
        entries[2].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'S', wxID_SAVEAS);
        entries[3].Set(wxACCEL_CTRL, (int)'E', wxID_EXPORT);
        wxAcceleratorTable accel(4, entries);
        SetAcceleratorTable(accel);

        // 通知パネルの作成と初期非表示
        notificationPanel = new SaveNotificationPanel(this, wxString::FromUTF8("保存完了"));
        notificationPanel->SetPosition(wxPoint(GetClientSize().GetWidth() - 220, 10));
        notificationPanel->Hide();

        wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
        topSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
        SetSizer(topSizer);
        Layout();

    }


private:
    wxNotebook* notebook;   // wxNotebook をメンバ変数として管理
    Tab1Panel* tab1Panel;   // Tab1Panel へのポインタ
    Tab2Panel* tab2Panel;   // Tab2Panel へのポインタ
    Tab3Panel* tab3Panel;   // Tab3Panel へのポインタ
    DynamicPawnPanel* pawnLinePanel; // pawnLinePanel へのポインタ
    wxString currentFilePath; //  ファイルパスを保存する変数

    SaveNotificationPanel* notificationPanel; // 通知用パネル

    // Tab1 と Tab3 の入力状態を1つの JSON ファイルに保存する。
    bool SaveAllToJson(const wxString& filePath)
    {
        try {
            json root;
            root["format"] = "SW25Manager";
            root["version"] = 1;
            root["tab1"] = tab1Panel->CreateJsonFromGUI();
            root["tab3"] = tab3Panel->CreateJsonFromGUI();
            std::ofstream file(filePath.ToStdString());
            if (!file.is_open()) return false;
            file << root.dump(4);
            file.flush();
            return file.good();
        }
        catch (const std::exception& e) {
            wxLogError("Failed to save application JSON: %s", e.what());
            return false;
        }
    }

    // JSONファイルを開く
    void OnOpenFile(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, wxString::FromUTF8("JSONファイルを開く"),  "", "", wxString::FromUTF8("JSONファイル (*.json)|*.json"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = openFileDialog.GetPath();
        std::ifstream file(filePath.ToStdString());
        if (!file) {
            wxMessageBox(wxString::FromUTF8("ファイルを開けませんでした"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
        }

        json jsonData;
        try {
            file >> jsonData;
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::FromUTF8("JSONファイルの形式が不正です。\n") + wxString::FromUTF8(e.what()), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
        }
        file.close();

        bool tab1Loaded = false;
        try {
            tab1Loaded = tab1Panel && tab1Panel->LoadFromJson(filePath);
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::FromUTF8("Tab1のデータを読み込めませんでした。\n") + wxString::FromUTF8(e.what()), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
        }
        if (!tab1Loaded) {
            wxMessageBox(wxString::FromUTF8("Tab1のデータを読み込めませんでした"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
        }
        if (jsonData.is_object() && jsonData.contains("tab3") && jsonData["tab3"].is_object()) {
            if (!tab3Panel->LoadFromJson(jsonData["tab3"])) {
                wxMessageBox(wxString::FromUTF8("Tab3のデータを完全には復元できませんでした"), wxString::FromUTF8("警告"), wxOK | wxICON_WARNING);
            }
        }

        currentFilePath = filePath; //  新しいファイルパスを保存
        notificationPanel->SetMessage(wxString::FromUTF8("ファイルの読み込み完了"));
        notificationPanel->ShowNotification();
        //wxMessageBox(wxString::FromUTF8("ファイルを読み込みました:\n") + wxString(jsonData.dump(4)), wxString::FromUTF8("成功"), wxOK | wxICON_INFORMATION);
    }

    // JSONファイルを保存
    void OnSaveFile(wxCommandEvent& event) {
        wxFileDialog saveFileDialog(this, wxString::FromUTF8("名前を付けて保存"), "", wxString::FromUTF8("新規キャラクター"), wxString::FromUTF8("JSONファイル (*.json)|*.json"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = saveFileDialog.GetPath();
        // 保存が成功したかを判定
        if (!SaveAllToJson(filePath)) {
            wxMessageBox(wxString::FromUTF8("ファイルの保存に失敗しました"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
            
        }

        currentFilePath = filePath; //  新しいファイルパスを保存
        notificationPanel->SetMessage(wxString::FromUTF8("名前を付けて保存完了"));
        notificationPanel->ShowNotification();
    }


    void OnSave(wxCommandEvent& event) {
        if (currentFilePath.IsEmpty()) {
            // ファイルパスが未設定なら「名前を付けて保存」を実行
            OnSaveFile(event);
        }
        else {
            
            if (!SaveAllToJson(currentFilePath)) {
                wxMessageBox(wxString::FromUTF8("ファイルの保存に失敗しました"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
                return;
            }
            // 既存のファイルに上書き保存
            notificationPanel->SetMessage(wxString::FromUTF8("上書き保存完了"));
            notificationPanel->ShowNotification();
        }

    }

    // ココフォリアへの出力
    void OnEXPORT(wxCommandEvent& event) {
        if (tab1Panel->CopyToCocofolia()){
            notificationPanel->SetMessage(wxString::FromUTF8("ココフォリア出力準備完了"));
            notificationPanel->ShowNotification();
        }
    }
    // void OnEXPORT(wxCommandEvent& event)
    // {
    //     notificationPanel->SetMessage(
    //         wxString::FromUTF8("EXPORTが呼ばれました")
    //     );
    //     notificationPanel->ShowNotification();
    // }

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
