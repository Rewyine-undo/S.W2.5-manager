#pragma warning(disable : 28251)
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

// �ۑ��ʒm�p�̊ȈՃp�l���N���X
class SaveNotificationPanel : public wxPanel {
public:
    SaveNotificationPanel(wxWindow* parent, const wxString& message)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(190, 40),
            wxBORDER_SIMPLE | wxSTAY_ON_TOP), timer(this)
    {
        SetBackgroundColour(wxColour(150, 200, 255, 128)); // ���邢��

        m_label = new wxStaticText(this, wxID_ANY, message, wxPoint(10, 10));
        m_label->SetForegroundColour(*wxBLACK);
        m_label->SetFont(wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, ""));
        m_label->SetForegroundColour(wxColour(255, 255, 255)); // ��
        Hide();

        // �^�C�}�[���~�܂������\���ɂ���
        Bind(wxEVT_TIMER, [this](wxTimerEvent&) {
            Hide();
            }, timer.GetId());
    }

    void ShowNotification(int milliseconds = 3000) {
        Show();
        Raise();  // �őO�ʂ�
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
        : wxFrame(nullptr, wxID_ANY, "�L�����N�^�[�Ǘ��A�v��", wxDefaultPosition, wxSize(630, 800)),
        notebook(nullptr), tab1Panel(nullptr), tab2Panel(nullptr), pawnLinePanel(nullptr)  // �����o�ϐ���������
    {
        // wxNotebook�i�^�u�j���쐬
        notebook = new wxNotebook(this, wxID_ANY);

        // Tab1Panel �̃C���X�^���X���ɍ쐬
        tab1Panel = new Tab1Panel(notebook);
        notebook->AddPage(tab1Panel, "�f�[�^�\��");

        // Tab2Panel ���쐬���A Tab1Panel ��n��
        tab2Panel = new Tab2Panel(notebook, notebook, tab1Panel);
        notebook->AddPage(tab2Panel, "�\�͍쐬");

        pawnLinePanel = new DynamicPawnPanel(notebook);
        notebook->AddPage(pawnLinePanel, "�㋉�퓬(GM����)");


        // ���j���[�o�[���쐬
        wxMenuBar* menuBar = new wxMenuBar();
        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(wxID_OPEN, "�t�@�C����ǂݍ���(Ctrl+F)");
        fileMenu->Append(wxID_SAVEAS, "���O��t���ăt�@�C����ۑ�(Ctrl+shift+S)");
        fileMenu->Append(wxID_SAVE, "�t�@�C�����㏑���ۑ�(Ctrl+S)");
        fileMenu->Append(wxID_EXIT, "�I��");
        menuBar->Append(fileMenu, "�t�@�C��");
        SetMenuBar(menuBar);

        // �C�x���g�n���h����ݒ�
        Bind(wxEVT_MENU, &MainFrame::OnOpenFile, this, wxID_OPEN);
        Bind(wxEVT_MENU, &MainFrame::OnSaveFile, this, wxID_SAVEAS);
        Bind(wxEVT_MENU, &MainFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

        // �V���[�g�J�b�g�L�[�ݒ�
        wxAcceleratorEntry entries[3];
        entries[0].Set(wxACCEL_CTRL, (int)'F', wxID_OPEN);
        entries[1].Set(wxACCEL_CTRL, (int)'S', wxID_SAVE);
        entries[2].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'S', wxID_SAVEAS);
        wxAcceleratorTable accel(3, entries);
        SetAcceleratorTable(accel);

        // �ʒm�p�l���̍쐬�Ə�����\��
        notificationPanel = new SaveNotificationPanel(this, "�ۑ�����");
        notificationPanel->SetPosition(wxPoint(GetClientSize().GetWidth() - 220, 10));
        notificationPanel->Hide();


    }


private:
    wxNotebook* notebook;   // wxNotebook �������o�ϐ��Ƃ��ĊǗ�
    Tab1Panel* tab1Panel;   // Tab1Panel �ւ̃|�C���^
    Tab2Panel* tab2Panel;   // Tab2Panel �ւ̃|�C���^
    DynamicPawnPanel* pawnLinePanel; // pawnLinePanel �ւ̃|�C���^
    wxString currentFilePath; //  �t�@�C���p�X��ۑ�����ϐ�

    SaveNotificationPanel* notificationPanel; // �ʒm�p�p�l��

    // JSON�t�@�C�����J��
    void OnOpenFile(wxCommandEvent& event) {
        wxFileDialog openFileDialog(this, "JSON�t�@�C�����J��", "", "", "JSON�t�@�C�� (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = openFileDialog.GetPath();
        std::ifstream file(filePath.ToStdString());
        if (!file) {
            wxMessageBox("�t�@�C�����J���܂���ł���", "�G���[", wxOK | wxICON_ERROR);
            return;
        }

        json jsonData;
        file >> jsonData;
        file.close();

        // Tab1Panel�̃C���X�^���X���擾���ALoadFromJson���Ăяo���ăf�[�^���Z�b�g
        if (tab1Panel) {
            tab1Panel->LoadFromJson(filePath);  // jsonData��n���ăZ�b�g
        }
        else {
            wxMessageBox("Tab1Panel�̃C���X�^���X��������܂���ł���", "�G���[", wxOK | wxICON_ERROR);
        }

        currentFilePath = filePath; //  �V�����t�@�C���p�X��ۑ�
        notificationPanel->SetMessage("�t�@�C���̓ǂݍ��݊���");
        notificationPanel->ShowNotification();
        //wxMessageBox("�t�@�C����ǂݍ��݂܂���:\n" + wxString(jsonData.dump(4)), "����", wxOK | wxICON_INFORMATION);
    }

    // JSON�t�@�C����ۑ�
    void OnSaveFile(wxCommandEvent& event) {
        wxFileDialog saveFileDialog(this, "���O��t���ĕۑ�", "", "�V�K�L�����N�^�[", "JSON�t�@�C�� (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }

        wxString filePath = saveFileDialog.GetPath();
        tab1Panel->SaveToJson(filePath);

        // �ۑ��������������𔻒�
        if (!tab1Panel->SaveToJson(filePath)) {
            wxMessageBox("�t�@�C���̕ۑ��Ɏ��s���܂���", "�G���[", wxOK | wxICON_ERROR);
            return;
            
        }

        currentFilePath = filePath; //  �V�����t�@�C���p�X��ۑ�
        notificationPanel->SetMessage("���O��t���ĕۑ�����");
        notificationPanel->ShowNotification();
    }


    void OnSave(wxCommandEvent& event) {
        if (currentFilePath.IsEmpty()) {
            // �t�@�C���p�X�����ݒ�Ȃ�u���O��t���ĕۑ��v�����s
            OnSaveFile(event);
        }
        else {
            
            if (!tab1Panel->SaveToJson(currentFilePath)) {
                wxMessageBox("�t�@�C���̕ۑ��Ɏ��s���܂���", "�G���[", wxOK | wxICON_ERROR);
            }
            // �����̃t�@�C���ɏ㏑���ۑ�
            notificationPanel->SetMessage("�㏑���ۑ�����");
            notificationPanel->ShowNotification();
        }


    }

    // �A�v�����I��
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
