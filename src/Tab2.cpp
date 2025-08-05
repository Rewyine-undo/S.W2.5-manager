#include "Tab2.h"
#include "Tab1.h" 
#include "utility.h"
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>


// �R���X�g���N�^
Tab2Panel::Tab2Panel(wxWindow* parent, wxNotebook* notebook, Tab1Panel* tab1Panel) : wxPanel(parent), notebook(notebook), tab1Panel(tab1Panel), resultsSizer(new wxBoxSizer(wxVERTICAL)) {
    // ���C�����C�A�E�g

    // �R���X�g���N�^�Ȃǂň�x�����Ăяo��
    //new wxLogWindow(this, "�f�o�b�O���O", true);

    // �t�H���g�ݒ�
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ���O���̓Z�N�V����
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* nameLabel = new wxStaticText(this, wxID_ANY, "���O: ");
    nameTextBox = new wxTextCtrl(this, wxID_ANY, "�V�K�L�����N�^�[", wxDefaultPosition, wxSize(100, -1));
    nameErrorText = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    nameErrorText->SetForegroundColour(*wxRED);
    nameErrorText->Hide();

    nameSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    nameSizer->Add(nameTextBox, 1);

    mainSizer->Add(nameSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(nameErrorText, 0, wxLEFT | wxRIGHT, 10);

    // �푰���X�g�{�b�N�X�Z�N�V����
    wxBoxSizer* speciesSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* speciesLabel = new wxStaticText(this, wxID_ANY, "�푰: ");
    wxArrayString speciesChoices;
    speciesChoices.Add("�I�����Ă�������");
    speciesChoices.Add("�l��");
    speciesChoices.Add("�G���t");
    speciesChoices.Add("�h���[�t");
    speciesChoices.Add("�^�r�b�g");
    speciesChoices.Add("���[���t�H�[�N");
    speciesChoices.Add("�i�C�g���A");
    speciesChoices.Add("���J���g");
    speciesChoices.Add("�����h���P��");
    speciesChoices.Add("�O���X�����i�[");
    speciesChoices.Add("�����A");
    speciesChoices.Add("�e�B�G���X");
    speciesChoices.Add("���v���J�[��");

    speciesListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(200, 100), speciesChoices, wxLB_SINGLE);
    speciesListBox->Bind(wxEVT_LISTBOX, &Tab2Panel::OnSpeciesSelected, this);
    speciesListBox->SetSelection(0);
    

    InitializeSpeciesData(); // �O���b�h�����ɕK�v�ȃf�[�^���ɏ�����


    speciesSizer->Add(speciesLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    speciesSizer->Add(speciesListBox, 1);

    mainSizer->Add(speciesSizer, 0, wxEXPAND | wxALL, 10);

    // ���܂�Z�N�V����
    wxFont font(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxBoxSizer* birthSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* birthLabel = new wxStaticText(this, wxID_ANY, "���܂�: ");
    wxStaticText* skillLabel = new wxStaticText(this, wxID_ANY, "�Z: ");
    skillSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    skillSpinCtrl->SetFont(font);  // �t�H���g�K�p
    wxStaticText* bodyLabel = new wxStaticText(this, wxID_ANY, "��: ");
    bodySpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    bodySpinCtrl->SetFont(font);  // �t�H���g�K�p
    wxStaticText* mindLabel = new wxStaticText(this, wxID_ANY, "�S: ");
    mindSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    mindSpinCtrl->SetFont(font);  // �t�H���g�K�p

    birthSizer->Add(birthLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(skillLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(skillSpinCtrl, 0, wxRIGHT, 10);
    birthSizer->Add(bodyLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(bodySpinCtrl, 0, wxRIGHT, 10);
    birthSizer->Add(mindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(mindSpinCtrl, 0);

    mainSizer->Add(birthSizer, 0, wxEXPAND | wxALL, 10);

    // �쐬�{�^��
    wxButton* createButton = new wxButton(this, wxID_ANY, "�쐬");
    createButton->Bind(wxEVT_BUTTON, &Tab2Panel::OnCreateButtonClicked, this);

    mainSizer->Add(createButton, 0, wxALIGN_CENTER | wxALL, 10);




    // �\����p�̎푰�\�R���e�i
    wxStaticBoxSizer* gridBox = new wxStaticBoxSizer(wxVERTICAL, this, "�푰�̐��܂�\");

    wxPanel* gridPanel = new wxPanel(this);
    wxBoxSizer* gridPanelSizer = new wxBoxSizer(wxVERTICAL);

    // �\���p�̃O���b�h�쐬
    for (const auto& [name, data] : speciesData) {
        int numRows = static_cast<int>(data.size());
        int numCols = 4; // ���܂�A�Z�\�A�\�͒l�A�o���_

        wxGrid* grid = new wxGrid(gridPanel, wxID_ANY);
        grid->SetMinSize(wxSize(500, 300));
        grid->CreateGrid(numRows, numCols);

        // �J�������x��
        grid->SetColLabelValue(0, "���܂�");
        grid->SetColLabelValue(1, "�������L�Z�\");
        grid->SetColLabelValue(2, "��b�\�͒l");
        grid->SetColLabelValue(3, "�����o���_");

        grid->SetRowLabelSize(0); // �s���x����\��

        // �e�Z���Ƀf�[�^���Z�b�g
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                if (col < static_cast<int>(data[row].size())) {
                    grid->SetCellValue(row, col, data[row][col]);
                    grid->SetReadOnly(row, col);  // �ҏW�s�ɂ���
                }
            }
        }

        // �Z���̎����T�C�Y����
        grid->AutoSizeColumns();
        // �K�v�ɉ����čs�̍�������������
        grid->AutoSizeRows();
        grid->Hide(); // �����͉B��
        speciesTables[name] = grid;
        gridPanelSizer->Add(grid, 1, wxEXPAND | wxALL, 5);
    }

    gridPanel->SetSizer(gridPanelSizer);
    gridBox->Add(gridPanel, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(gridBox, 1, wxEXPAND | wxALL, 10);






    // ���C�A�E�g��K�p
    this->SetSizer(mainSizer);
}
// �f�X�g���N�^
Tab2Panel::~Tab2Panel() {
    // resultsSizer �����
    delete resultsSizer;
}


void Tab2Panel::InitializeSpeciesData()
{
    // ��F�u�l�ԁv�̃f�[�^������
    speciesData["�l��"] = {
        // �����u1
        {"�����@�t", "�}�M�e�b�N", "�Z8    ��4    �S9", "2000�_"},
        {"���p�t", "�\�[�T���[", "�Z6    ��5    �S10", "2000�_"},
        {"�y��m", "�X�J�E�g&�t�F���T�[", "�Z10    ��7    �S4", "2000�_"},
        {"��ʐl", "�Ȃ�", "�Z7    ��7    �S7", "3000�_"},
        {"�b��", "�t�@�C�^�[or�O���b�v���[", "�Z8    ��10    �S4", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z4    ��8    �S9", "2000�_"},
        {"����p�t", "�R���W�����[", "�Z7    ��4    �S10", "2000�_"},
        {"�`����", "�Ȃ�", "�Z2d    ��2d    �S2d", "3000�_"},
        //�����u2
        {"�ˎ�", "�V���[�^�[", "�Z9    ��5    �S7", "2500�_"},
        {"���m", "�t�F���T�[", "�Z9    ��6    �S6", "2500�_"},
        {"�����m", "�O���b�v���[", "�Z8    ��8    �S5", "2000�_"},
        {"��m", "�t�@�C�^�[", "�Z7    ��9    �S5", "2000�_"},
        {"���̎m", "�G���n���T�[", "�Z6    ��8    �S7", "2500�_"},
        {"�̐l", "�o�[�h", "�Z5    ��7    �S9", "2500�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z5    ��6    �S10", "2000�_"}
    };
    // �G���t
    speciesData["�G���t"] = {
        {"���m", "�t�F���T�[", "�Z12    ��5    �S9", "2500�_"},
        {"��t", "�Z�[�W&�����W���[", "�Z10    ��5    �S11", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z9    ��5    �S12", "2000�_"},
        {"����p�t", "�R���W�����[", "�Z9    ��4    �S13", "2000�_"},
        {"���p�t", "�\�[�T���[", "�Z10    ��3    �S13", "2000�_"},
        {"�ˎ�", "�V���[�^�[", "�Z13    ��5    �S8", "2500�_"},

        {"���̎m", "�G���n���T�[", "�Z11    ��5    �S10", "2500�_"},
        {"����", "�X�J�E�g", "�Z12    ��4    �S10", "2500�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z10    ��2    �S14", "2000�_"},
        {"�����m", "�O���b�v���[", "�Z11    ��4    �S11", "2000�_"},
        {"��V���l", "�o�[�h", "�Z9    ��3    �S14", "2500�_"}
    };
    // �h���[�t
    speciesData["�h���[�t"] = {
        {"�ˎ�", "�V���^�[", "�Z6    ��8    �S6", "2500�_"},
        {"��m", "�t�@�C�^�[", "�Z4    ��11    �S5", "2000�_"},
        {"�����m", "�O���b�v���[", "�Z5    ��10    �S5", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z4    ��7    �S9", "2000�_"},
        {"�����@�t", "�}�M�e�b�N", "�Z6    ��7    �S7", "2000�_"},

        {"�w��", "�Z�[�W", "�Z3    ��8    �S9", "2500�_"},
        {"�앚", "�����W���[", "�Z6    ��9    �S5", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z5    ��9    �S6", "2500�_"},
        {"��V���l", "�o�[�h", "�Z4    ��8    �S8", "2500�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z5    ��6    �S9", "2000�_"}
    };
    // �^�r�b�g
    speciesData["�^�r�b�g"] = {
        {"����p�t", "�R���W�����[", "�Z6    ��6    �S10", "2000�_"},
        {"���p�t", "�\�[�T���[", "�Z5    ��7    �S10", "2000�_"},
        {"�w��", "�Z�[�W", "�Z5    ��8    �S9", "2500�_"},
        {"�����@�t", "�}�M�e�b�N", "�Z8    ��5    �S9", "2000�_"},

        {"�����t", "�\�[�T���[&�R���W�����[", "�Z5    ��6    �S11", "1000�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z7    ��5    �S10", "2000�_"},
        {"��V���l", "�o�[�h", "�Z6    ��7    �S9", "2500�_"},
        {"��t", "�����W���[", "�Z7    ��6    �S9", "2500�_"}
    };
    // ���[���t�H�[�N
    speciesData["���[���t�H�[�N"] = {
        {"�w��", "�Z�[�W", "�Z8    ��10    �S8", "2500�_"},
        {"�ˎ�", "�V���[�^�[", "�Z12    ��8    �S6", "2500�_"},
        {"��m", "�t�@�C�^�[or�O���b�v���[", "�Z9    ��12    �S5", "2000�_"},
        {"�����@�t", "�}�M�e�b�N", "�Z12    ��8    �S6", "2000�_"},
        {"���p�t", "�\�[�T���[", "�Z9    ��8    �S9", "2000�_"},

        {"����", "�X�J�E�g", "�Z12    ��6    �S8", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z10    ��8    �S8", "2500�_"},
        {"�y��m", "�t�F���T�[", "�Z11    ��9    �S6", "2500�_"},
        {"��V���l", "�o�[�h", "�Z8    ��9    �S9", "2500�_"},
        {"����p�t", "�R���W�����[", "�Z7    ��9    �S10", "2000�_"}
    };
    // �i�C�g���A
    speciesData["�i�C�g���A"] = {
        {"���p�t", "�\�[�T���[", "�Z5    ��13    �S12", "2000�_"},
        {"�b��", "�t�@�C�^�[or�O���b�v���[", "�Z7    ��15    �S8", "2000�_"},
        {"�y��m", "�t�F���T�[&�X�J�E�g", "�Z11    ��13    �S6", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z6    ��14    �S10", "2000�_"},
        {"�����@�t", "�}�M�e�b�N", "�Z9    ��9    �S12", "2000�_"},

        {"��V���l", "�o�[�h", "�Z8    ��13    �S9", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z9    ��14    �S7", "2500�_"},
        {"�ˎ�", "�V���[�^�[", "�Z10    ��10    �S10", "2500�_"},
        {"�앚", "�����W���[", "�Z9    ��12    �S9", "2500�_"},
        {"����p�t", "�R���W�����[", "�Z6    ��11    �S13", "2000�_"}
    };
    // ���J���g
    speciesData["���J���g"] = {
        {"����", "�X�J�E�g", "�Z13    ��5    �S7", "2500�_"},
        {"��m", "�t�@�C�^�[", "�Z10    ��9    �S6", "2000�_"},
        {"�����m", "�O���b�v���[", "�Z11    ��7    �S7", "2000�_"},
        {"�y��m", "�t�F���T�[", "�Z12    ��6    �S7", "2500�_"},
        {"�앚", "�����W���[", "�Z9    ��8    �S8", "2500�_"},

        {"��V���l", "�o�[�h", "�Z8    ��9    �S8", "2500�_"},
        {"�ˎ�", "�V���[�^�[", "�Z11    ��8    �S6", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z10    ��8    �S7", "2500�_"},
        {"�w��", "�Z�[�W", "�Z10    ��7    �S8", "2500�_"},
        {"�_��", "�v���[�X�g", "�Z9    ��7    �S9", "2000�_"}
    };
    // �����h���P��
    speciesData["�����h���P��"] = {
        {"�앚", "�����W���[", "�Z6    ��12    �S7", "2500�_"},
        {"�����m", "�O���b�v���[", "�Z6    ��13    �S6", "2000�_"},
        {"��m", "�t�@�C�^�[", "�Z5    ��14    �S6", "2000�_"},
        {"���l", "�Z�[�W", "�Z5    ��11    �S9", "2500�_"},
        {"�_��", "�v���[�X�g", "�Z4    ��13    �S8", "2000�_"},

        {"�ˎ�", "�V���[�^�[", "�Z7    ��12    �S6", "2500�_"},
        {"�y��m", "�t�F���T�[", "�Z6    ��11    �S8", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z5    ��12    �S8", "2500�_"},
        {"���@�g��", "�\�[�T���[or�R���W�����[", "�Z4    ��12    �S9", "2000�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z3    ��12    �S10", "2000�_"}
    };
    // �O���X�����i�[
    speciesData["�O���X�����i�["] = {
        {"�����l", "�X�J�E�g", "�Z13    ��0    �S12", "2500�_"},
        {"�y��m", "�t�F���T�[", "�Z14    ��1    �S10", "2500�_"},
        {"�앚", "�����W���[", "�Z12    ��1    �S12", "2500�_"},
        {"�ˎ�", "�V���[�^�[", "�Z14    ��0    �S11", "2500�_"},
        {"��l", "�Z�[�Wor�o�[�h", "�Z12    ��0    �S13", "2500�_"},

        {"�w��", "�Z�[�W", "�Z11    ��1    �S13", "2500�_"},
        {"�����m", "�O���b�v���[", "�Z14    ��2    �S9", "2000�_"},
        {"���l", "�Ȃ�", "�Z11    ��2    �S12", "3000�_"},
        {"����", "�t�F���T�[&�X�J�E�g", "�Z15    ��0    �S10", "2000�_"},
        {"��V���l", "�o�[�h", "�Z12    ��0    �S13", "2500�_"}
    };
    // �����A
    speciesData["�����A"] = {
        {"�앚", "�����W���[", "�Z9    ��8    �S12", "2500�_"},
        {"�_��", "�v���[�X�g", "�Z8    ��8    �S13", "2000�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z8    ��7    �S14", "2000�_"},
        {"���p�t", "�\�[�T���[", "�Z8    ��6    �S15", "2000�_"},
        {"����p�t", "�R���W�����[", "�Z7    ��6    �S16", "2000�_"},

        {"�ˎ�", "�V���[�^�[", "�Z10    ��7    �S12", "2500�_"},
        {"�y��m", "�t�F���T�[", "�Z10    ��8    �S11", "2500�_"},
        {"�����t", "�\�[�T���[&�R���W�����[", "�Z8    ��5    �S16", "1000�_"},
        {"��V���l", "�o�[�h", "�Z7    ��7    �S15", "2500�_"},
        {"���̎m", "�G���n���T�[", "�Z9    ��9    �S11", "2500�_"}
    };
    // �e�B�G���X
    speciesData["�e�B�G���X"] = {
        {"�R��", "���C�_�[", "�Z10    ��11    �S7", "2500�_"},
        {"�����m", "�O���b�v���[", "�Z9    ��13    �S6", "2000�_"},
        {"��m", "�t�@�C�^�[", "�Z8    ��12    �S8", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z7    ��12    �S9", "2000�_"},
        {"���@�g��", "�\�[�T���[", "�Z6    ��12    �S10", "2000�_"},

        {"�ˎ�", "�V���[�^�[", "�Z11    ��12    �S5", "2500�_"},
        {"����", "�X�J�E�g", "�Z10    ��10    �S8", "2500�_"},
        {"���@��m", "�t�@�C�^�[&�\�[�T���[", "�Z9    ��11    �S8", "1000�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z7    ��11    �S10", "2000�_"},
        {"�w��", "�Z�[�W", "�Z8    ��11    �S9", "2500�_"}
    };
    // ���v���J�[��
    speciesData["���v���J�[��"] = {
        {"�y��m", "�t�F���T�[", "�Z13    ��5    �S5", "2500�_"},
        {"�ˎ�", "�V���[�^�[", "�Z12    ��6    �S5", "2500�_"},
        {"����", "�X�J�E�g", "�Z14    ��4    �S5", "2500�_"},
        {"�d���g��", "�t�F�A���[�e�C�}�[", "�Z11    ��4    �S8", "2000�_"},
        {"�B���p�t", "�A���P�~�X�g", "�Z11    ��5    �S7", "2500�_"},

        {"�앚", "�����W���[", "�Z12    ��5    �S6", "2500�_"},
        {"��l", "�Z�[�W&�o�[�h", "�Z13    ��4    �S6", "2000�_"},
        {"�����@�t", "�}�M�e�b�N", "�Z12    ��4    �S7", "2000�_"},
        {"�_��", "�v���[�X�g", "�Z10    ��5    �S8", "2000�_"},
        {"�����t", "�\�[�T���[or�R���W�����[", "�Z11    ��3    �S9", "2000�_"}
    };  
}



void Tab2Panel::OnSpeciesSelected(wxCommandEvent& event) {
    int selection = speciesListBox->GetSelection();
    if (selection == wxNOT_FOUND || selection == 0) return;

    wxString selectedSpecies = speciesListBox->GetString(selection);
    //wxLogMessage("�I�����ꂽ�푰: %s", selectedSpecies);

    //wxLogMessage("�����܂ŗ���");
    //wxLogDebug("�I�����ꂽ�푰: %s", selectedSpecies);


    for (auto& [name, grid] : speciesTables) {
        if (name == selectedSpecies) {
            grid->Show();
            grid->SetMinSize(wxSize(500, 300)); // �T�C�Y�w��
            //wxLogMessage("�\���Ώ�: %s", name);
        }
        else {
            grid->Hide();
        }
    }

    // �O���b�h�̐e�i�ʏ�� gridPanel�j�ɂ����C�A�E�g���w��
    if (!speciesTables.empty()) {
        auto anyGrid = speciesTables.begin()->second;
        if (anyGrid) {
            anyGrid->GetParent()->Layout();
        }
    }

    Layout();  // Tab2Panel���̂̃��C�A�E�g���X�V
    Refresh();
}






void Tab2Panel::OnCreateButtonClicked(wxCommandEvent&) {
        // ���O�̓��̓`�F�b�N
        wxString name = nameTextBox->GetValue();
        if (name.IsEmpty()) {
            nameErrorText->SetLabel("���̃t�B�[���h����͂��Ă�������");
            nameErrorText->Show();
            this->Layout();
            return;
        }
        nameErrorText->Hide();
        
        // �푰�̑I���`�F�b�N
        int speciesIndex = speciesListBox->GetSelection();
        if (speciesIndex == 0) {
            wxMessageBox("�푰��I�����Ă�������", "�G���[", wxOK | wxICON_ERROR);
            return;
        }
        // �푰�̕����̎擾
        wxString speciesName = speciesListBox->GetString(speciesIndex);
        
        // ���s���ʂ𐶐����鏈��

        trialResults.clear(); // �ȑO�̎��s���폜

        // 3��̎��s�����s
        for (int i = 0; i < 3; ++i) {
            TrialResult result;

            result.name = std::string(name.mb_str());
            result.species = std::string(speciesName.mb_str());

            // �Z�A�́A�S�̒l���擾
            result.born[0] = skillSpinCtrl->GetValue();
            result.born[1] = bodySpinCtrl->GetValue();
            result.born[2] = mindSpinCtrl->GetValue();

            // �푰�ɉ�����A~F���v�Z
            CalculateAttributes(speciesIndex, result.dicenumber, result.diceeffect, result.ability, result.impurity, result.feature);
            std::cout << "After CalculateAttributes: result.dicenumber = " << result.dicenumber << std::endl;

            // �e�\�͒l�ƃ{�[�i�X���v�Z
            CalculateAbilityValues(result);

            trialResults.push_back(result);
        }
        // ���ʂ�\��
        DisplayResults();
    }
    

void Tab2Panel::CalculateAttributes(int speciesIndex, int& dicenumber, int& diceeffect, int* ability,int& impurity, std::string& feature) {
    // �푰�ɉ�����A~F���v�Z (���X�g�{�b�N�X�̃C���f�b�N�X�ƑΉ�)
    switch (speciesIndex) {
    case 1: // �l��
        dicenumber = 12;
        diceeffect = 0;
        feature = "[���̉���/�^���ϓ]]";
        impurity = 0;
        for (int i = 0; i < 6; ++i) ability[i] = RollDice(2, 0);
        break;
    case 2: // �G���t
        dicenumber = 11;
        diceeffect = 0;
        impurity = 0;
        feature = "[�Î�][���̉���/�D������]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0); // A,B
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 0); // C,D
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0); // E,F   
        break;
    case 3: // �h���[�t
        dicenumber = 10;
        diceeffect = 1.2;
        impurity = 0;
        feature = "[�Î�][���̉���/���g]";
        ability[0] = RollDice(2, 6); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 6);
        break;
    case 4: // �^�r�b�g
        dicenumber = 9;
        diceeffect = 0.6667;
        impurity = 0;
        feature = "[��Z��]";
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 6); ability[5] = RollDice(2, 0);
        break;
    case 5: // ���[���t�H�[�N
        dicenumber = 10;
        diceeffect = 0;
        impurity = 0;
        feature =  "[�Î�][HP�ϊ�]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(1, 0);
        break;
    case 6: // �i�C�g���A
        dicenumber = 10;
        diceeffect = 0;
        impurity = 1;
        feature =  "[�ٖe][��_(����)]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(1, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0);
        break;
    case 7: // ���J���g
        dicenumber = 8;
        diceeffect = 1.125;
        impurity = 0;
        feature =  "[�Î�(�b�ϖe)][�b�ϖe]";
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 3);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(1, 6); ability[5] = RollDice(1, 0);
        break;
    case 8: // �����h���P��
        dicenumber = 10;
        diceeffect = 0.6;
        impurity = 0;
        feature =  "[�؂̔畆][�K��������][���̉���/���̗�]";
        ability[0] = RollDice(1, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 0);
        break;
    case 9: // �O���X�����i�[
        dicenumber = 10;
        diceeffect = 1.2;
        impurity = 0;
        feature =  "[�}�i�s����][����A���Ƃ̈ӎv�a��]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 6);
        break;
    case 10: // �����A
        dicenumber = 7;
        diceeffect = 0.8571;
        impurity = 0;
        feature =  "[�ɖ΂��鐶��]";
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(1, 0);
        break;
    case 11: // �e�B�G���X
        dicenumber = 10;
        diceeffect = 0.6;
        impurity = 0;
        feature = "[�ʂ������ӎ�]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(1, 3);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 3);
        break;
    case 12: // ���v���J�[��
        dicenumber = 11;
        diceeffect = 0;
        impurity = 0;
        feature = "[�Î�][���������][�p�Ȃ��E�l]";
        ability[0] = RollDice(2, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0);
        break;
    default:
        std::cerr << "�G���[: �����Ȏ푰�C���f�b�N�X" << std::endl;
        dicenumber = 0;
        diceeffect = 0;
    }
    std::cout << "CalculateAttributes: dicenumber = " << dicenumber << std::endl;
 }

void Tab2Panel::CalculateAbilityValues(TrialResult& result) {
    // �Z, ��, �S + A~F �Ɋ�Â��\�͒l���v�Z
    result.attributes[0] = result.born[0] + result.ability[0]; // ��p�x = �Z + A
    result.attributes[1] = result.born[0] + result.ability[1]; // �q���x = �Z + B
    result.attributes[2] = result.born[1] + result.ability[2]; // �ؗ� = �� + C
    result.attributes[3] = result.born[1] + result.ability[3]; // ������ = �� + D
    result.attributes[4] = result.born[2] + result.ability[4]; // �m�� = �S + E
    result.attributes[5] = result.born[2] + result.ability[5]; // ���_�� = �S + F

    result.pretotal = 0;
    result.total = 0;
    for (int i = 0; i < 6; ++i) {
        result.bonuses[i] = result.attributes[i] / 6;
        result.pretotal += result.ability[i];
        result.total += result.attributes[i];
        std::cout << "result.total (for loop��): " << result.total << std::endl;
    }

    // int���Z�̐��������p���ď�����܈ʂ�؂�グ
    int average = (result.pretotal*10000) / result.dicenumber;
    // ������x10^4�Ŋ��邱�ƂŌ��ʂ𐳂�
    float average2 = average / 10000.0f;
    result.average = average2 - result.diceeffect;
}



void Tab2Panel::DisplayResults() {
    // �_�C�A���O�^�C�g���̍쐬
    wxString dialogTitle;
    if (!trialResults.empty()) {
        dialogTitle = wxString::Format("���s����: %s", trialResults[0].name);
    }
    else {
        dialogTitle = "���s����";
    }

    // �_�C�A���O�E�B���h�E���쐬
    wxDialog* resultDialog = new wxDialog(this, wxID_ANY, dialogTitle, wxDefaultPosition, wxSize(506, 450));
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);

    // 3��̎��s���ʂ�\��
    for (size_t i = 0; i < trialResults.size(); ++i) {
        const TrialResult& result = trialResults[i];

        // ���s�w�b�_�[�ƃ{�^���̃��C�A�E�g
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        // ���s�w�b�_�[���쐬
        wxStaticText* header = new wxStaticText(resultDialog, wxID_ANY,
            wxString::Format("���s%d: %s", (int)(i + 1), result.species));
        headerSizer->Add(header, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

        // �u���̔\�͒l�ŐV�K�쐬�v�{�^��
        wxButton* createButton = new wxButton(resultDialog, wxID_ANY, "���̔\�͒l�ŐV�K�쐬");
        createButton->Bind(wxEVT_BUTTON, [this, result, resultDialog](wxCommandEvent&) {
            // tab1Panel �����������m�F
            if (tab1Panel) {
                std::vector<int> characterData = {
                    result.born[0], result.born[1], result.born[2],  // �Z�E�́E�S
                    result.ability[0], result.ability[1], result.ability[2], result.ability[3], result.ability[4], result.ability[5], // A�`F
                    result.impurity
                };
                                
                tab1Panel->SetCharacterData(result.name, result.species, result.feature, characterData);
            }
            else {
                wxMessageBox("Tab1Panel��������܂���B", "�G���[", wxOK | wxICON_ERROR);
            }

            // �_�C�A���O�����O�Ƀ^�u��؂�ւ��Ȃ�
            resultDialog->EndModal(wxID_OK);  // �_�C�A���O�����
            });
        headerSizer->Add(createButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

        // ���s�w�b�_�[���_�C�A���O�ɒǉ�
        dialogSizer->Add(headerSizer, 0, wxEXPAND | wxALL, 5);

        // �O���b�h���쐬
        wxGrid* grid = new wxGrid(resultDialog, wxID_ANY);
        grid->CreateGrid(4, 6); // �s���Ɨ񐔂�ύX

        // ��ƍs�̃��x�����\��
        grid->SetColLabelSize(0);
        grid->SetRowLabelSize(0);

        // �Z���̌����ݒ�
        grid->SetCellSize(0, 0, 1, 2);
        grid->SetCellSize(0, 2, 1, 2);
        grid->SetCellSize(0, 4, 1, 2);
        grid->SetCellSize(3, 0, 1, 6);

        // �Z���̒�����
        int rows = grid->GetNumberRows();
        int cols = grid->GetNumberCols();
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                grid->SetCellAlignment(row, col, wxALIGN_CENTER, wxALIGN_CENTER);
            }
        }

        // ���ʂ��O���b�h�ɃZ�b�g
        grid->SetCellValue(0, 0, wxString::Format("�Z: %d", result.born[0]));
        grid->SetCellValue(0, 2, wxString::Format("��: %d", result.born[1]));
        grid->SetCellValue(0, 4, wxString::Format("�S: %d", result.born[2]));

        grid->SetCellValue(1, 0, wxString::Format("A: %d", result.ability[0]));
        grid->SetCellValue(1, 1, wxString::Format("B: %d", result.ability[1]));
        grid->SetCellValue(1, 2, wxString::Format("C: %d", result.ability[2]));
        grid->SetCellValue(1, 3, wxString::Format("D: %d", result.ability[3]));
        grid->SetCellValue(1, 4, wxString::Format("E: %d", result.ability[4]));
        grid->SetCellValue(1, 5, wxString::Format("F: %d", result.ability[5]));

        grid->SetCellValue(2, 0, wxString::Format("��p: %d", result.attributes[0]));
        grid->SetCellValue(2, 1, wxString::Format("�q��: %d", result.attributes[1]));
        grid->SetCellValue(2, 2, wxString::Format("�ؗ�: %d", result.attributes[2]));
        grid->SetCellValue(2, 3, wxString::Format("����: %d", result.attributes[3]));
        grid->SetCellValue(2, 4, wxString::Format("�m��: %d", result.attributes[4]));
        grid->SetCellValue(2, 5, wxString::Format("���_: %d", result.attributes[5]));

        grid->SetCellValue(3, 0, wxString::Format("�_�C�X����: %.4f / �\�͒l���v: %d", result.average, result.total));

        grid->EnableEditing(false); // �ҏW�s�ɐݒ�
        // �O���b�h���_�C�A���O�ɒǉ�
        dialogSizer->Add(grid, 0, wxEXPAND | wxALL, 5);
    }

    // ����{�^��
    wxButton* closeButton = new wxButton(resultDialog, wxID_OK, "����");
    dialogSizer->Add(closeButton, 0, wxALIGN_CENTER | wxALL, 10);

    resultDialog->SetSizer(dialogSizer);
    resultDialog->ShowModal();

    // �_�C�A���O��������Ƀ^�u��؂�ւ�
    if (notebook) {
        notebook->SetSelection(0);  // Tab1Panel �ɐ؂�ւ�
    }

    resultDialog->Destroy();  // �_�C�A���O�����ۂɃ����������
}
