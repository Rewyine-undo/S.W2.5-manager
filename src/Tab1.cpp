#define WXJSON_USE_EXTERNAL
#include "Tab1.h"
#include "Tab2.h"
#include <algorithm>
#include <wx/filedlg.h>
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <numeric> 

using json = nlohmann::json;


Tab1Panel::Tab1Panel(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    {
    SetScrollRate(5, 5);  // �X�N���[���̑��x��ݒ�

    sizer = new wxBoxSizer(wxVERTICAL);

    // JSON�t�@�C���ǂݍ��ݗp��UI
    wxStaticText* instruction = new wxStaticText(this, wxID_ANY, "JSON�t�@�C����ǂݍ��݁A�f�[�^��\�����܂��B");
    //wxTextCtrl* displayArea = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(550, 300), wxTE_MULTILINE | wxTE_READONLY);
    sizer->Add(instruction, 0, wxALL, 10);
    //sizer->Add(displayArea, 1, wxEXPAND | wxALL, 10);

    
    // ���O�̃e�L�X�g�{�b�N�X
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(new wxStaticText(this, wxID_ANY, "���O: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    nameTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(nameTextBox, 0, wxALIGN_CENTER| wxRIGHT, 3);
    nameSizer->Add(new wxStaticText(this, wxID_ANY, "PL: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    PLTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(PLTextBox, 0, wxALIGN_CENTER);

    // �푰�̃e�L�X�g�{�b�N�X
    wxBoxSizer* speciesSizer = new wxBoxSizer(wxHORIZONTAL);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, "�푰: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    speciesTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1));
    speciesTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    speciesSizer->Add(speciesTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, "�N��: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ageTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(ageTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, "����: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    sexTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(sexTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, "�q��: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ImpurityTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(32, -1));
    speciesSizer->Add(ImpurityTextBox, 0, wxALIGN_CENTER);

    // �푰�����̃e�L�X�g�{�b�N�X
    wxBoxSizer* spfeatureSizer = new wxBoxSizer(wxHORIZONTAL);
    spfeatureSizer->Add(new wxStaticText(this, wxID_ANY, "�푰����: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    spfeatureTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
    spfeatureSizer->Add(spfeatureTextBox, 0, wxALIGN_CENTER);
    

    // �M�̃e�L�X�g�{�b�N�X
    wxBoxSizer* faithSizer = new wxBoxSizer(wxHORIZONTAL);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, "���܂�: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    birthTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(65, -1));
    faithSizer->Add(birthTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, "�M��: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    faithTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
    faithSizer->Add(faithTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, "�����N: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    rankTextBox = new wxTextCtrl(this, wxID_ANY, "�[", wxDefaultPosition, wxSize(80, -1));
    faithSizer->Add(rankTextBox, 0, wxALIGN_CENTER);

    sizer->Add(nameSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(speciesSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(spfeatureSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(faithSizer, 0, wxEXPAND | wxALL, 3);

    wxStaticText* abi = new wxStaticText(this, wxID_ANY, "---�\�͒l---");
    sizer->Add(abi, 0, wxALL, 10);
    wxStaticText* comment = new wxStaticText(this, wxID_ANY, "�����͂Ȃǂ͋Z�\�A����̒��ōő�l���Q��");
    comment->SetForegroundColour(*wxRED); // �ԐF�ɐݒ�
    sizer->Add(comment, 0, wxALIGN_RIGHT | wxBOTTOM, 5);

    // �eSizer�������тɐݒ�
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main2Sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main1Sizer = new wxBoxSizer(wxVERTICAL);
    // bornTextBoxes�i�Z�E�́E�S�j�̏�����
    wxBoxSizer* bornSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bornLabels[3] = { "(�Z", "(��", "(�S" };
    for (int i = 0; i < 3; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, bornLabels[i] + " ");
        bornSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        bornTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(40, 68));
        bornSizer->Add(bornTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������

        // �e�L�X�g�𒆉������ɐݒ�
        bornTextBoxes[i]->SetWindowStyleFlag(wxTE_CENTER);
        bornTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    // abilityTextBoxes�iA�`F�j�̏�����
    wxBoxSizer* abilitySizer = new wxBoxSizer(wxVERTICAL);
    const wxString abilityLabels[6] = { "+A", "+B", "+C", "+D", "+E", "+F" };
    for (int i = 0; i < 6; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, abilityLabels[i] + " ");
        abilitySizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        abilityTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(40, -1));
        abilitySizer->Add(abilityTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        abilityTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    //�����l�̍���
    wxBoxSizer* growthSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, "+����)");
        growthSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �X�s���{�^����ǉ�
        growthSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        growthSizer->Add(growthSpinCtrl[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        growthSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::UpdateAttribute, this);
    }

    // attributeTextBoxes�i��p�`���_�j�̏�����
    wxBoxSizer* attributeSizer = new wxBoxSizer(wxVERTICAL);
    const wxString attributeLabels[6] = { "=(��p�x", "=(�q���x", "=(�ؗ�", "=(������", "=(�m��", "=(���_��" };
    for (int i = 0; i < 6; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, attributeLabels[i] + " ");
        attributeSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        attributeTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(60, -1));
        attributeSizer->Add(attributeTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        attributeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::abilityChange, this);
    }
    
    //�����l�̍���
    wxBoxSizer* increaseSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, "+����)");
        increaseSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �X�s���{�^����ǉ�
        increaseSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        increaseSizer->Add(increaseSpinCtrl[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        increaseSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::Updatebonus, this);
    }

    // bonusTextBoxes�i��p�`���_�{�[�i�X�j�̏�����
    wxBoxSizer* bonusSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bonusLabels[6] = { "/6����p�xB", "/6���q���xB", "/6���ؗ�B", "/6��������B", "/6���m��B", "/6�����_��B" };
    for (int i = 0; i < 6; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, bonusLabels[i] + " ");
        bonusSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        bonusTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(75, -1));
        bonusSizer->Add(bonusTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        bonusTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    
    // HP,MP,��R�͂̏�����
    wxBoxSizer* hpSizer = new wxBoxSizer(wxVERTICAL);
    const wxString hpLabels[4] = { "HP", "MP", "������R��", "���_��R��"};
    for (int i = 0; i < 4; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, hpLabels[i] + " ");
        hpSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        hpTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(60, -1));
        hpSizer->Add(hpTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
     }
    const wxString hitLabels[4] = {"������", "�ǉ�D", "����", "�h��_" };
    for (int i = 0; i < 4; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, hitLabels[i] + " ");
        hpSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        hitSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        hpSizer->Add(hitSpinCtrl[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }


    // HP,MP,����p�b�P�[�W�̏�����
    wxBoxSizer* packSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* packlabel = new wxStaticText(this, wxID_ANY, "����p�b�P�[�W: ");
    packSizer->Add(packlabel, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    const wxString packLabels[7] = {"�Z�I" , "�^��" , "�ώ@" , "�m��", "�����m��", "�搧��", "����"};
    for (int i = 0; i < 7; ++i) {
        // ���x����ǉ�
        wxStaticText* label = new wxStaticText(this, wxID_ANY, packLabels[i] + " ");
        packSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�

        // �e�L�X�g�{�b�N�X��ǉ�
        packSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        packSizer->Add(packSpinCtrl[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������

    }


    // �����тɂ���
    main2Sizer->Add(bornSizer, 0, wxALIGN_CENTER);   
    main2Sizer->Add(abilitySizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(growthSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(attributeSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(increaseSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(bonusSizer, 0, wxALIGN_CENTER | wxLEFT, 15);

    // �ړ���
    wxBoxSizer* moveSizer = new wxBoxSizer(wxHORIZONTAL);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "�ړ���: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    moveTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(moveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "m"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "*3= �S�͈ړ�: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    fullmoveTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(fullmoveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "m"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "�����ړ�: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    restmoveTextBox = new wxTextCtrl(this, wxID_ANY, "3", wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(restmoveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, "m"), 0, wxALIGN_CENTER_VERTICAL);


    main1Sizer->Add(main2Sizer, 0, wxALIGN_CENTER| wxBOTTOM, 25);
    main1Sizer->Add(moveSizer, 0, wxALIGN_CENTER);


    mainSizer->Add(main1Sizer, 0, wxALIGN_TOP);
    mainSizer->Add(hpSizer, 0, wxALIGN_TOP | wxLEFT, 30);
    mainSizer->Add(packSizer, 0, wxALIGN_CENTER | wxLEFT, 10);

    // mainSizer��eSizer�ɒǉ�
    sizer->Add(mainSizer, 0, wxEXPAND | wxALL, 2);




    wxStaticText* tec = new wxStaticText(this, wxID_ANY, "---�Z�\�E���Z---");
    sizer->Add(tec, 0, wxALL, 10);


    // �eSizer�������тɐݒ�
    wxBoxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* ssubSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sub1Sizer = new wxBoxSizer(wxHORIZONTAL);

    // �Z�\�̏�����
    wxBoxSizer* techSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* techlabel = new wxStaticText(this, wxID_ANY, "�Z�\ ");
    techSizer->Add(techlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        techTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        techSizer->Add(techTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        techTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
     }

    // �Z�\�e�[�u���̏�����
    wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* tablelabel = new wxStaticText(this, wxID_ANY, "�e�[�u�� ");
    tableSizer->Add(tablelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // ���W�I�{�^����ǉ�
        wxBoxSizer* subtableSizer = new wxBoxSizer(wxHORIZONTAL);
        tableradioA[i] = new wxRadioButton(this, wxID_ANY, "A", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        tableradioB[i] = new wxRadioButton(this, wxID_ANY, "B");
        
        subtableSizer->Add(tableradioA[i], 0, wxALIGN_CENTER );
        subtableSizer->Add(tableradioB[i], 0, wxALIGN_CENTER );
        tableSizer->Add(subtableSizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 4.5);
        // �C�x���g��o�^
        tableradioA[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
        tableradioB[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
    }
    

    // �Z�\���x���̏�����
    wxBoxSizer* techlevelSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* techlevellabel = new wxStaticText(this, wxID_ANY, "���x�� ");
    techlevelSizer->Add(techlevellabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �X�s���{�^����ǉ�
        techlevelSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 15, 0);
        techlevelSizer->Add(techlevelSpinCtrl[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        techlevelSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnSpinCtrlChange, this);
     }

    // �K�v�o���l�̏�����
    wxBoxSizer* expSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* explabel = new wxStaticText(this, wxID_ANY, "�g�p�o���l ");
    expSizer->Add(explabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        expTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(60, -1));
        expSizer->Add(expTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        expTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updateallexp, this);
    }

    // �����тɂ���
    sub1Sizer->Add(techSizer, 0, wxALIGN_CENTER);   
    sub1Sizer->Add(tableSizer, 0, wxALIGN_CENTER | wxRIGHT|wxLEFT, 3);
    sub1Sizer->Add(techlevelSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    sub1Sizer->Add(expSizer, 0, wxALIGN_CENTER | wxLEFT, 2);

    // �`���҃��x���A�擾�o���l�A���v�g�p�o���l
    wxBoxSizer* allexpSizer = new wxBoxSizer(wxHORIZONTAL);
       
    allexpSizer->Add(new wxStaticText(this, wxID_ANY, "�擾�o���l: "), 0, wxALIGN_CENTER_VERTICAL);
    getexpSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000000, 0);
    getexpSpinCtrl->SetIncrement(50); // �ω�����50�ɐݒ�
    getexpSpinCtrl->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    getexpSpinCtrl->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(getexpSpinCtrl, 0, wxALIGN_CENTER | wxRIGHT, 6);
    
    allexpSizer->Add(new wxStaticText(this, wxID_ANY, "���v�g�p�o���l: "), 0, wxALIGN_CENTER_VERTICAL );
    allexpTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(60, -1));
    allexpTextBox->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(allexpTextBox, 0, wxALIGN_CENTER);

    // �`���҃��x���A�擾�o���l�A���v�g�p�o���l
    wxBoxSizer* levelSizer = new wxBoxSizer(wxHORIZONTAL);
    levelSizer->Add(new wxStaticText(this, wxID_ANY, "�`���҃��x��: "), 0, wxALIGN_CENTER_VERTICAL );
    levelTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(40, -1));
    levelTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    levelSizer->Add(levelTextBox, 0, wxALIGN_CENTER);

    warningText = new wxStaticText(this, wxID_ANY, "�擾�o���l�𒴉߂��Ă��܂��I", wxPoint(10, 10));
    warningText->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    warningText->SetForegroundColour(*wxRED); // �ԐF�ɐݒ�
    warningText->Hide(); // ������Ԃł͔�\���ɂ���
    levelSizer->Add(warningText, 0, wxALIGN_CENTER| wxLEFT, 10);



    ssubSizer->Add(sub1Sizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(allexpSizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(levelSizer, 0, wxALIGN_LEFT);


    wxBoxSizer* sub2Sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* talentSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText * talentlabel = new wxStaticText(this, wxID_ANY, "�퓬���Z");
    talentSizer->Add(talentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 8; ++i) {
        wxString talentText = wxString::Format("%d: ", 2 * i + 1); // ���l�𕶎���ɕϊ�
        // �e�L�X�g�{�b�N�X��ǉ�
        talentTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, talentText, wxDefaultPosition, wxSize(120, -1));
        talentSizer->Add(talentTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        talentTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* abstSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* abstlabel = new wxStaticText(this, wxID_ANY, "���ʊT�v");
    abstSizer->Add(abstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 8; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        abstTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        abstSizer->Add(abstTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    // �����тɂ���
    sub2Sizer->Add(talentSizer, 0, wxALIGN_CENTER);
    sub2Sizer->Add(abstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);

    //sub1Sizer��sub2Seizer��subSeizer�ɂ܂Ƃ߂�
    subSizer->Add(ssubSizer, 0, wxALIGN_TOP);
    subSizer->Add(sub2Sizer, 0, wxALIGN_TOP | wxLEFT, 20);

    // subSizer��eSizer�ɒǉ�
    sizer->Add(subSizer, 0, wxEXPAND | wxLEFT, 3);


    // �L�q���̐ݒ�
    freespaceSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* freeSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* freetalentSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* freetalentlabel = new wxStaticText(this, wxID_ANY, "�����擾�Ȃ�");
    freetalentSizer->Add(freetalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    freetalentTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freetalentTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    freetalentSizer->Add(freetalentTextBox, 0, wxALIGN_LEFT | wxLEFT, 3);

    wxBoxSizer* freeabstSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* freeabstlabel = new wxStaticText(this, wxID_ANY, "���ʊT�v");
    freeabstSizer->Add(freeabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    freeabstTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freeabstSizer->Add(freeabstTextBox, 0, wxALIGN_LEFT);

    freeSizer->Add(freetalentSizer, 0, wxALIGN_CENTER);
    freeSizer->Add(freeabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    wxBoxSizer* magicSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* magictalentSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* magictalentlabel = new wxStaticText(this, wxID_ANY, "�悭�g�����@�Ȃ�");
    magictalentSizer->Add(magictalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    magictalentTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magictalentSizer->Add(magictalentTextBox, 0, wxALIGN_LEFT);

    wxBoxSizer* magicabstSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* magicabstlabel = new wxStaticText(this, wxID_ANY, "MP, ���ʊT�v");
    magicabstSizer->Add(magicabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    magicabstTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magicabstSizer->Add(magicabstTextBox, 0, wxALIGN_LEFT);

    magicSizer->Add(magictalentSizer, 0, wxALIGN_CENTER);
    magicSizer->Add(magicabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    wxBoxSizer* battleSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* battlelabel = new wxStaticText(this, wxID_ANY, "�퓬�֘A����");
    wxStaticText* battlelabel2 = new wxStaticText(this, wxID_ANY, "(�󂯂����ʂȂ�)");
    battleSizer->Add(battlelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    battleSizer->Add(battlelabel2, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    battleTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 131), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    battleSizer->Add(battleTextBox, 0, wxALIGN_LEFT);

        
    freespaceSizer->Add(freeSizer, 0, wxALIGN_CENTER);
    freespaceSizer->Add(magicSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    freespaceSizer->Add(battleSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    
    

    sizer->Add(freespaceSizer, 0, wxALIGN_LEFT | wxTOP, 15);

    placeholderSizer = new wxBoxSizer(wxVERTICAL); // ���̑��Z�\�̓���ꏊ

    otherskillSizer = new wxBoxSizer(wxHORIZONTAL);
    placeholderSizer->Add(otherskillSizer, 0, wxALIGN_LEFT);
    sizer->Add(placeholderSizer, 0, wxALL, 5);


    // �eSizer�������тɐݒ�
    trioSizer = new wxBoxSizer(wxHORIZONTAL);

    // ����̏�����
    wxBoxSizer* langSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* langlabel = new wxStaticText(this, wxID_ANY, "����");
    langSizer->Add(langlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    const wxString langtext[10] = { "���Ջ��ʌ�", "�ėp�ؑ���", "�n����(�u�����C�g)", "�푰����( )", "���@������", "�����@������", "�d����", "�_�I������", "���_��", ""};
    for (int i = 0; i < 10; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        langTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, langtext[i], wxDefaultPosition, wxSize(110, -1));
        langSizer->Add(langTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    // ��b�̏�����
    wxBoxSizer* speakSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* speaklabel = new wxStaticText(this, wxID_ANY, "��b");
    speakSizer->Add(speaklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 10; ++i) {
        // �`�F�b�N�{�b�N�X��ǉ�
        speakcheckBoxes[i] = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        speakSizer->Add(speakcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 4.5);
    }
    speakcheckBoxes[0]->SetValue(true);

    // �Ǖ��̏�����
    wxBoxSizer* readSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* readlabel = new wxStaticText(this, wxID_ANY, "�Ǖ�");
    readSizer->Add(readlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 10; ++i) {
        // �`�F�b�N�{�b�N�X��ǉ�
        readcheckBoxes[i] = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        readSizer->Add(readcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 4.5);
    }
    readcheckBoxes[0]->SetValue(true);

    //sub1Sizer��sub2Seizer��subSeizer�ɂ܂Ƃ߂�
    trioSizer->Add(langSizer, 0, wxALIGN_TOP | wxLEFT, 2);
    trioSizer->Add(speakSizer, 0, wxALIGN_TOP | wxLEFT, 1);
    trioSizer->Add(readSizer, 0, wxALIGN_TOP | wxLEFT, 1);

    wxBoxSizer* otherSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* otherlabel = new wxStaticText(this, wxID_ANY, "�o���A���̑������Ȃ�");
    otherSizer->Add(otherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ���x���̉��ɃX�y�[�X��ǉ�
    otherTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(360, 235), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    otherSizer->Add(otherTextBox, 0, wxALIGN_LEFT);
    
    // weaponSizer��trioSizer�ɒǉ�
    trioSizer->Add(otherSizer, 0, wxALIGN_TOP | wxLEFT, 5);

    // subSizer��eSizer�ɒǉ�
    sizer->Add(trioSizer, 0, wxEXPAND | wxTOP, 15);


    wxStaticText* item = new wxStaticText(this, wxID_ANY, "---����E�����i---");
    sizer->Add(item, 0, wxALL, 10);

    // �����i���̏�����
    wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

    // ���헓�̏�����
    wxBoxSizer* weaponSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* weaponnameSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponnamelabel = new wxStaticText(this, wxID_ANY, "����");
    weaponnameSizer->Add(weaponnamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponnameTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 46));
        weaponnameSizer->Add(weaponnameTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    wxBoxSizer* weaponuseSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponuselabel = new wxStaticText(this, wxID_ANY, "�p�@");
    weaponuseSizer->Add(weaponuselabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponuseTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weaponuseSizer->Add(weaponuseTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    wxBoxSizer* weaponstrSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponstrlabel = new wxStaticText(this, wxID_ANY, "�K��");
    weaponstrSizer->Add(weaponstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponstrTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(40, -1));
        weaponstrSizer->Add(weaponstrTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    wxBoxSizer* weaponhitSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponhitlabel = new wxStaticText(this, wxID_ANY, "������");
    weaponhitSizer->Add(weaponhitlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponhitTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(40, -1));
        weaponhitSizer->Add(weaponhitTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        weaponhitTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* weapondmSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weapondmlabel = new wxStaticText(this, wxID_ANY, "�З�");
    weapondmSizer->Add(weapondmlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weapondmTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weapondmSizer->Add(weapondmTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }


    wxBoxSizer* weaponcritSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponcritlabel = new wxStaticText(this, wxID_ANY, "C�l");
    weaponcritSizer->Add(weaponcritlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponcritTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "11", wxDefaultPosition, wxSize(50, -1));
        weaponcritSizer->Add(weaponcritTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }


    wxBoxSizer* weaponaddSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponaddlabel = new wxStaticText(this, wxID_ANY, "�ǉ�D");
    weaponaddSizer->Add(weaponaddlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 6; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponaddTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
        weaponaddSizer->Add(weaponaddTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        weaponaddTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }


    wxBoxSizer* weaponotherSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* weaponotherlabel = new wxStaticText(this, wxID_ANY, "���l");
    weaponotherSizer->Add(weaponotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        weaponotherTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(140, 46), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
        weaponotherSizer->Add(weaponotherTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }
    
    //weapon�n����܂Ƃ߂�
    weaponSizer->Add(weaponnameSizer, 0, wxALIGN_CENTER);
    weaponSizer->Add(weaponuseSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponhitSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weapondmSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponcritSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponaddSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    // �h��Ə������������킹��
    wxBoxSizer* armorSizer = new wxBoxSizer(wxHORIZONTAL);

    // �h��̏�����
    wxBoxSizer* subarmorSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* armornameSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* armornamelabel = new wxStaticText(this, wxID_ANY, "�h��");
    armornameSizer->Add(armornamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    const wxString armortext[3] = { "�Z", "��", "���̑�" };
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        armornameTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, armortext[i] + ":", wxDefaultPosition, wxSize(110, -1));
        armornameSizer->Add(armornameTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    wxBoxSizer* armorstrSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* armorstrlabel = new wxStaticText(this, wxID_ANY, "�K��");
    armorstrSizer->Add(armorstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        armorstrTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        armorstrSizer->Add(armorstrTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    wxBoxSizer* armorevadeSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* armorevadelabel = new wxStaticText(this, wxID_ANY, "����C��");
    armorevadeSizer->Add(armorevadelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        armorevadeTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
        armorevadeSizer->Add(armorevadeTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        armorevadeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* armorblockSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* armorblocklabel = new wxStaticText(this, wxID_ANY, "�h��_");
    armorblockSizer->Add(armorblocklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        armorblockTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(40, -1));
        armorblockSizer->Add(armorblockTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        armorblockTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* armorotherSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* armorotherlabel = new wxStaticText(this, wxID_ANY, "���l");
    armorotherSizer->Add(armorotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 3; ++i) {
        // �e�L�X�g�{�b�N�X��ǉ�
        armorotherTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        armorotherSizer->Add(armorotherTextBoxes[i], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
    }

    //armor�n����܂Ƃ߂�
    subarmorSizer->Add(armornameSizer, 0, wxALIGN_CENTER);
    subarmorSizer->Add(armorstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorevadeSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorblockSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);



    // ���K�̃e�L�X�g�{�b�N�X
    wxBoxSizer* moneySizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* poketSizer = new wxBoxSizer(wxHORIZONTAL);
    poketSizer->Add(new wxStaticText(this, wxID_ANY, "������: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    poketSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000, 0);
    poketSizer->Add(poketSpinCtrl, 0, wxALIGN_CENTER);
    poketSizer->Add(new wxStaticText(this, wxID_ANY, "G"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* debtSizer = new wxBoxSizer(wxHORIZONTAL);
    debtSizer->Add(new wxStaticText(this, wxID_ANY, "�a��/�؋�: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    debtTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(60, -1));
    debtSizer->Add(debtTextBox, 0, wxALIGN_CENTER);
    debtSizer->Add(new wxStaticText(this, wxID_ANY, "G"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* glorySizer = new wxBoxSizer(wxHORIZONTAL);
    glorySizer->Add(new wxStaticText(this, wxID_ANY, "���_�_: "), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    glorySpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000, 0);
    glorySizer->Add(glorySpinCtrl, 0, wxALIGN_CENTER);
    glorySizer->Add(new wxStaticText(this, wxID_ANY, "�_"), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    moneySizer->Add(poketSizer, 0, wxALIGN_LEFT | wxBOTTOM, 2);
    moneySizer->Add(debtSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);
    moneySizer->Add(glorySizer, 0, wxALIGN_LEFT);

    armorSizer->Add(subarmorSizer, 0, wxALIGN_CENTER);
    armorSizer->Add(moneySizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    

    itemSizer->Add(weaponSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);
    itemSizer->Add(armorSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);

    sizer->Add(itemSizer, 0, wxEXPAND | wxLEFT, 2);

    wxBoxSizer* equipSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* acesSizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* acesSizer2 = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* aceslabel = new wxStaticText(this, wxID_ANY, "�e���ʂ̑����i");
    acesSizer1->Add(aceslabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    const wxString acesLabels[12] = { "��", "��", "��", "��", "�w��", "�E��", "����", "��", "��", "���̑�", "���̑�2", "���̑�3" };
    for (int i = 0; i < 12; ++i) { //�����i��
        acesTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, acesLabels[i] + ": ", wxDefaultPosition, wxSize(150, -1));
        acesSizer1->Add(acesTextBoxes[i], 0, wxALIGN_LEFT);
    }
    acesTextBoxes[10]->Hide();
    acesTextBoxes[11]->Hide();
    
    // ���x����ǉ�
    wxStaticText* aceslabel2 = new wxStaticText(this, wxID_ANY, "���ʊT�v");
    acesSizer2->Add(aceslabel2, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    for (int i = 0; i < 12; ++i) { // �����i�̌���
        acesTextBoxes2[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(175, -1));
        acesSizer2->Add(acesTextBoxes2[i], 0, wxALIGN_LEFT);
        
    }
    acesTextBoxes2[10]->Hide();
    acesTextBoxes2[11]->Hide();
    equipSizer->Add(acesSizer1, 0, wxALIGN_LEFT);
    equipSizer->Add(acesSizer2, 0, wxALIGN_LEFT);
    


    wxBoxSizer* bagSizer = new wxBoxSizer(wxVERTICAL);
    // ���x����ǉ�
    wxStaticText* baglabel = new wxStaticText(this, wxID_ANY, "�����A�C�e���Ȃ�");
    bagSizer->Add(baglabel, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ���x���̉��ɃX�y�[�X��ǉ�
    bagTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(250, 300), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    bagSizer->Add(bagTextBox, 0, wxALIGN_LEFT);

    equipSizer->Add(bagSizer, 0, wxLEFT, 5);
    sizer->Add(equipSizer, 0, wxEXPAND);




    SetSizer(sizer);
    FitInside();
}


void Tab1Panel::SetCharacterData(const wxString& name, const wxString& species, const wxString& feature, const std::vector<int>& data){
    if (data.size() < 5) return;  // �f�[�^��������Ȃ���Ή������Ȃ�

    nameTextBox->SetValue(name);
    speciesTextBox->SetValue(species);
    spfeatureTextBox->SetValue(feature);
    ImpurityTextBox->SetValue(wxString::Format("%d", data[9]));

    // �Z�E�́E�S
    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->SetValue(wxString::Format("%d", data[i]));
    }
    // A�`F
    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->SetValue(wxString::Format("%d", data[3 + i]));
    }
    //// ��p�E�q���E�ؗ́E�����E�m�́E���_
    //for (int i = 0; i < 6; ++i) {
    //    attributeTextBoxes[i]->SetValue(wxString::Format("%d", data[9 + i]));
    //}
}

void Tab1Panel::UpdateAttribute(wxCommandEvent& event) {
    long bornVal[3], abilityVal[6], growthVal[6], increaseVal[6]; // �擾����l���i�[����z��

    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->GetValue().ToLong(&bornVal[i]);
    }

    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->GetValue().ToLong(&abilityVal[i]);
        growthVal[i] = growthSpinCtrl[i]->GetValue();
        increaseVal[i] = increaseSpinCtrl[i]->GetValue();
    }

    // ���v�� �eattributeTextBoxes �ɃZ�b�g
    attributeTextBoxes[0]->SetValue(wxString::Format("%ld", bornVal[0] + abilityVal[0] + growthVal[0]));
    attributeTextBoxes[1]->SetValue(wxString::Format("%ld", bornVal[0] + abilityVal[1] + growthVal[1]));
    attributeTextBoxes[2]->SetValue(wxString::Format("%ld", bornVal[1] + abilityVal[2] + growthVal[2]));
    attributeTextBoxes[3]->SetValue(wxString::Format("%ld", bornVal[1] + abilityVal[3] + growthVal[3]));
    attributeTextBoxes[4]->SetValue(wxString::Format("%ld", bornVal[2] + abilityVal[4] + growthVal[4]));
    attributeTextBoxes[5]->SetValue(wxString::Format("%ld", bornVal[2] + abilityVal[5] + growthVal[5]));

}

void Tab1Panel::Updatebonus(wxCommandEvent& event) {
    long attributeVal[6], increaseVal[6]; // �擾����l���i�[����z��

    for (int i = 0; i < 6; ++i) {
        attributeTextBoxes[i]->GetValue().ToLong(&attributeVal[i]);
        increaseVal[i] = increaseSpinCtrl[i]->GetValue();
    }

    // �{�[�i�X�v�Z���ebonusTextBox�ɃZ�b�g
    for (int i = 0; i < 6; ++i) {
        bonusTextBoxes[i]->SetValue(wxString::Format("%ld", (attributeVal[i] + increaseVal[i]) / 6));
    }
}

void Tab1Panel::Updatelevel(wxCommandEvent& event) {
    long techlevel[6]; //�擾����l���i�[����z��
    int advlevel=0;
    for (int i = 0; i < 6; ++i) {
        techlevel[i] = techlevelSpinCtrl[i]->GetValue();
    }
    // �z��̒��̍ő�l���擾
    advlevel = *std::max_element(techlevel, techlevel + 6);

    levelTextBox->SetValue(wxString::Format("%ld", advlevel));
}

void Tab1Panel::Updatehp(wxCommandEvent& event) {
    int lv = 0, aglity = 0, vital = 0, magic = 0, hit = 0, evade = 0, damage = 0, mplv=0, mlv = 0, armor = 0, tarmor = 0 ,weaponhit = 0, weapondamage = 0, armorevade = 0, tarmorevade = 0, tech = 0, mov = 0, sage = 0 ;
    long bonus[6], hitally[6], evadeally[6], damageally[6],  magically[6], weaponhitally[6], weapondamageally[6];
    std::fill(magically, magically + 6, 0), std::fill(hitally, hitally + 6, 0), std::fill(evadeally, evadeally + 6, 0), std::fill(damageally, damageally + 6, 0);
    bool enhancerAdded = false;  // �ǉ��ς݂��ǂ����̃t���O

    levelTextBox->GetValue().ToInt(&lv);
    attributeTextBoxes[1]->GetValue().ToInt(&aglity);
    attributeTextBoxes[3]->GetValue().ToInt(&vital);
    attributeTextBoxes[5]->GetValue().ToInt(&magic);

    for (int i = 0; i < 6; ++i) {
		bonusTextBoxes[i]->GetValue().ToLong(&bonus[i]); // �{�[�i�X���擾
    }

    int evadetrue = 0;
    wxBoxSizer* newSizer = new wxBoxSizer(wxHORIZONTAL); // lv�K���Z�\�̌��o��
    wxBoxSizer* lvskillSizer1 = new wxBoxSizer(wxVERTICAL); // �e�L�X�g�{�b�N�X������
    wxBoxSizer* lvskillSizer2 = new wxBoxSizer(wxVERTICAL); // �e�L�X�g�{�b�N�X������

    for (int i = 0; i < 6; ++i) {
		weaponhitTextBoxes[i]->GetValue().ToLong(&weaponhitally[i]); // ����̖����͂��擾
        weaponaddTextBoxes[i]->GetValue().ToLong(&weapondamageally[i]); // ����̒ǉ�D���擾

        std::string className = techTextBoxes[i]->GetValue().ToStdString(); // �Z�\�����擾
        if (className.empty()) {
            //this->Layout();
            continue; // �Z�\������̏ꍇ�̓X�L�b�v
        }

        else if (className == "�t�@�C�^�[" || className == "�O���b�v���[" || className == "�t�F���T�[") {
            hitally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[0];
            evadeally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[1];
			damageally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[2];

		} // ��m�n�Z�\ �����́A���́A�ǉ�D

        else if (className == "�V���[�^�[") {
            hitally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[0];
            damageally[i] = techlevelSpinCtrl[i]->GetValue(); //�ؗ�B�Ȃ�
            
            for (int i = 0; i < 8; ++i) { // ����̔���
                std::string talentName = talentTextBoxes[i]->GetValue().ToStdString(); // ���Z�����擾
                if (talentName.find("�ˎ�̑̏p") != std::string::npos){
					evadetrue = 1; // �ˎ�̑̏p������ꍇ
                    break;
                }
            }
            if (evadetrue == 1) {
                evadeally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[1];
            }
        } // �V���[�^�[ �����́A���͓͂���

        else if (className == "�\�[�T���[" || className == "�R���W�����[" || className == "�v���[�X�g" || className == "�}�M�e�b�N" || className == "�t�F�A���[�e�C�}�[") {
            magically[i] = techlevelSpinCtrl[i]->GetValue();
        }// ���@�g���n�Z�\

        else if (className == "�X�J�E�g" || className == "�����W���[") {
            tech = techlevelSpinCtrl[i]->GetValue() + bonus[0]; // �Z�I
            mov = techlevelSpinCtrl[i]->GetValue() + bonus[1]; // �^���A�搧��
        }// �搧�Z�\

        else if (className == "�Z�[�W" || className == "���C�_�[") {
			sage = techlevelSpinCtrl[i]->GetValue() + bonus[4]; //�ώ@�A�m���A�����m��
        }// �����m���Z�\

        //else if (className == "�G���n���T�[" && !enhancerAdded) {
        //    // �S�̂��܂Ƃ߂�ꎞ�I�ȃ��b�p�[sizer
        //    wxBoxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);
        //    newSizer->Add(new wxStaticText(this, wxID_ANY, "�K�����Z"), 0);

        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText1 = wxString::Format("%d: ", j + 1); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        enhanceskillTextBoxes1[j] = new wxTextCtrl(this, wxID_ANY, otherskillText1, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(enhanceskillTextBoxes1[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //     }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText2 = wxString::Format("%d: ", j + 9); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        enhanceskillTextBoxes2[j] = new wxTextCtrl(this, wxID_ANY, otherskillText2, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(enhanceskillTextBoxes2[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    // otherskillSizer���ƒǉ�
        //    wrapperSizer->Add(otherskillSizer, 0);
        //                 
        //    placeholderSizer->Add(wrapperSizer, 0);//�Ō�� wrapper �� placeholderSizer �ɒǉ��I
        //    this->Layout();

        //    enhancerAdded = true;  // ��x�ƒǉ�����Ȃ��悤�ɂ���
        //}

        //else if (className == "�o�[�h") {
        //    placeholderSizer->Add(new wxStaticText(this, wxID_ANY, "�K����́E�I��"), 0, wxALL, 5);
        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText = wxString::Format("%d: ", j + 1); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText = wxString::Format("%d: ", j + 9); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    this->Layout();
        //}

        //else if (className == "���C�_�[") {
        //    // �S�̂��܂Ƃ߂�ꎞ�I�ȃ��b�p�[sizer
        //    wxBoxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);
        //    wrapperSizer->Add(new wxStaticText(this, wxID_ANY, "�K���R�|"), 0);

        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText1 = wxString::Format("%d: ", j + 1); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        enhanceskillTextBoxes1[j] = new wxTextCtrl(this, wxID_ANY, otherskillText1, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(enhanceskillTextBoxes1[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText2 = wxString::Format("%d: ", j + 9); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        enhanceskillTextBoxes2[j] = new wxTextCtrl(this, wxID_ANY, otherskillText2, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(enhanceskillTextBoxes2[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    // otherskillSizer���ƒǉ�
        //    wrapperSizer->Add(otherskillSizer, 0);

        //    placeholderSizer->Add(wrapperSizer, 0);//�Ō�� wrapper �� placeholderSizer �ɒǉ��I
        //    this->Layout();
        //}
         
        //else if (className == "�A���P�~�X�g") {
        //    placeholderSizer->Add(new wxStaticText(this, wxID_ANY, "�K�����p"), 0, wxALL, 5);
        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText = wxString::Format("%d: ", j + 1); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText = wxString::Format("%d: ", j + 9); // ���l�𕶎���ɕϊ�
        //        //�e�L�X�g�{�b�N�X��ǉ�
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // ���������ł͂Ȃ�������
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    this->Layout();
        //}

        else {
            //this->Layout();
            continue; // �Z�\������̏ꍇ�̓X�L�b�v
        }

    }
    // �z��̒��̍ő�l���擾
	hit = *std::max_element(hitally, hitally + 6);
    evade = *std::max_element(evadeally, evadeally + 6);
    mplv = std::accumulate(magically, magically + 6, 0);
    mlv = *std::max_element(magically, magically + 6);
    damage = *std::max_element(damageally, damageally + 6);

    weaponhit = *std::max_element(weaponhitally, weaponhitally + 6);
    weapondamage = *std::max_element(weapondamageally, weapondamageally + 6);

    for (int i = 0; i < 3; ++i) {
		armorblockTextBoxes[i]->GetValue().ToInt(&armor); // �h��̖h��_���擾
        tarmor += armor;
        armorevadeTextBoxes[i]->GetValue().ToInt(&armorevade); // �h��̉���C�����擾
		tarmorevade += armorevade;
    }

	std::string species = speciesTextBox->GetValue().ToStdString(); // �푰�����擾
    if (species == "�����h���P��") {
        tarmor += 1;
	} // �����h���P���̎푰�����ɂ��h��_+1
    else if (species == "���v���J�[��") {
        acesTextBoxes[10]->Show();
        acesTextBoxes2[10]->Show();
        if (lv > 5) {
            acesTextBoxes[11]->Show();
            acesTextBoxes2[11]->Show();
        }
        else {
            acesTextBoxes[11]->Hide();
            acesTextBoxes2[11]->Hide();
        }
        this->Layout();
    } // ���v���J�[���̎푰�����ɂ��lv6���炻�̑��̕���+1, lv11


    for (int i = 0; i < 8; ++i) { // �펞���Z�ɂ��\�͒l�̏C��
        std::string talentName = talentTextBoxes[i]->GetValue().ToStdString(); // ���Z�����擾
        if (talentName.empty()) continue; // �Z�\������̏ꍇ�̓X�L�b�v

        else if (talentName.find("�����΂�") != std::string::npos) {
            restmoveTextBox->SetValue(wxString::Format("10")); // �����΂�������ꍇ
        }
        else if (talentName.find("����s���T") != std::string::npos) {
            evade += 1; // ����s���T������ꍇ
        }
        else if (talentName.find("����s���U") != std::string::npos) {
            evade += 2; // ����s���U������ꍇ 
        }
        else if (talentName.find("�拭") != std::string::npos) {
            vital += 15; // �拭������ꍇ
        }
        else if (talentName.find("���拭") != std::string::npos) {
            vital += 15; // ���拭������ꍇ
        }
        else if (talentName.find("����K�nA") != std::string::npos) {
            weapondamage += 1; // ����K�nA������ꍇ
        }
        else if (talentName.find("����K�nS") != std::string::npos) {
            weapondamage += 2; // ����K�nS������ꍇ
        }
        else if (talentName.find("�h��K�nA") != std::string::npos) {
            tarmor += 1; // �h��K�nA������ꍇ
        }
        else if (talentName.find("�h��K�nS") != std::string::npos) {
            tarmor += 2; // �h��K�nS������ꍇ
        }
        else if (talentName.find("���͋����T") != std::string::npos) {
            bonus[4] += 1; // ���͋����T������ꍇ
        }
        else if (talentName.find("���͋����U") != std::string::npos) {
            bonus[4] += 2; // ���͋����U������ꍇ
        }
        else if (talentName.find("���������T") != std::string::npos) {
            weaponhit += 1; // ���������T������ꍇ
        }
        else if (talentName.find("���������U") != std::string::npos) {
            weaponhit += 2; // ���������U������ꍇ
        }
        else if (talentName.find("�L���p�V�e�B") != std::string::npos) {
             magic+= 15; // �L���p�V�e�B������ꍇ
        }

    }
    
    std::string freetalentName = freetalentTextBox->GetValue().ToStdString(); // �����擾�̓��Z�����擾
    if (freetalentName.find("�^�t�l�X") != std::string::npos) {
		vital += 15; // �^�t�l�X������ꍇ
    }

    // �`���҃��x���Ōv�Z�ł��鍀��
    hpTextBoxes[0]->SetValue(wxString::Format("%ld", (3 * lv + vital))); // Hp
    hpTextBoxes[2]->SetValue(wxString::Format("%ld", (lv + bonus[3]))); // ������R��
    hpTextBoxes[3]->SetValue(wxString::Format("%ld", (lv + bonus[5]))); // ���_��R��

    // ���@�Z�\���d�l���鍀��
    if (species == "�O���X�����i�[") {
        hpTextBoxes[1]->SetValue(wxString::Format("%ld", (0)));  
    } // �O���X�����i�[��MP0
    else {
        hpTextBoxes[1]->SetValue(wxString::Format("%ld", (3 * mplv + magic)));  // Mp
    }


    hitSpinCtrl[0]->SetValue(wxString::Format("%ld", (hit + weaponhit)));// ������
    hitSpinCtrl[1]->SetValue(wxString::Format("%ld", (damage + weapondamage)));// �ǉ�D
    hitSpinCtrl[2]->SetValue(wxString::Format("%ld", (evade + tarmorevade)));// ����
	hitSpinCtrl[3]->SetValue(wxString::Format("%ld", (tarmor)));// �h��_

    packSpinCtrl[0]->SetValue(wxString::Format("%ld", (tech)));// �Z�I
    packSpinCtrl[1]->SetValue(wxString::Format("%ld", (mov)));// �^��
    packSpinCtrl[2]->SetValue(wxString::Format("%ld", (sage)));// �ώ@
    packSpinCtrl[3]->SetValue(wxString::Format("%ld", (sage)));// �m��
    packSpinCtrl[4]->SetValue(wxString::Format("%ld", (sage)));// �����m��
    packSpinCtrl[5]->SetValue(wxString::Format("%ld", (mov)));// �搧��
	packSpinCtrl[6]->SetValue(wxString::Format("%ld", (mlv + bonus[4])));// ����
    
    
    moveTextBox->SetValue(wxString::Format("%ld", aglity)); // �ړ���
    fullmoveTextBox->SetValue(wxString::Format("%ld", 3*aglity)); // �S�͈ړ���
}



void Tab1Panel::Updateexp(wxCommandEvent& event) {
    long techlevelVal[6], tableVal[6]; // �擾����l���i�[����z��

    for (int i = 0; i < 6; ++i) {
        techlevelVal[i] = techlevelSpinCtrl[i]->GetValue();
        
        switch (techlevelVal[i]) {
            case 1:
                tableVal[i] = 500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 500;
                }
                break;
            case 2:
                tableVal[i] = 1500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 500;
                }
                break;
            case 3:
                tableVal[i] = 2500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 1000;
                }
                break;
            case 4:
                tableVal[i] = 4000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 1000;
                }
                break;
            case 5:
                tableVal[i] = 5500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 1500;
                }
                break;
            case 6:
                tableVal[i] = 7500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 2000;
                }
                break;
            case 7:
                tableVal[i] = 10000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 2500;
                }
                break;
            case 8:
                tableVal[i] = 13000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 3500;
                }
                break;
            case 9:
                tableVal[i] = 17000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 4500;
                }
                break;
            case 10:
                tableVal[i] = 22000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 5500;
                }
                break;
            case 11:
                tableVal[i] = 28000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 7000;
                }
                break;
            case 12:
                tableVal[i] = 35500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 8500;
                }
                break;
            case 13:
                tableVal[i] = 44500;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 10000;
                }
                break;
            case 14:
                tableVal[i] = 55000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 11500;
                }
                break;
            case 15:
                tableVal[i] = 67000;
                if (tableradioA[i]->GetValue()) {
                    tableVal[i] += 13000;
                }
                break;
            default:
                tableVal[i] = 0;
                break;
        }
    }

    // exp�v�Z���eexpTextBox�ɃZ�b�g
    for (int i = 0; i < 6; ++i) {
        expTextBoxes[i]->SetValue(wxString::Format("%ld", tableVal[i]));
    }
}

void Tab1Panel::Updateallexp(wxCommandEvent& event) {
    long expVal[6]; // �擾����l���i�[����z��
    int allexp{};
    for (int i = 0; i < 6; ++i) {
        expTextBoxes[i]->GetValue().ToLong(&expVal[i]);
        allexp += expVal[i];
    }
    allexpTextBox->ChangeValue(wxString::Format("%ld", allexp));
    //allexpTextBox->Update(); // UI���X�V����

    // �蓮�ŃC�x���g�𔭐�������
    wxCommandEvent compareevent(wxEVT_COMMAND_TEXT_UPDATED, allexpTextBox->GetId());
    compareevent.SetEventObject(allexpTextBox);  // �C�x���g�I�u�W�F�N�g��ݒ�
    allexpTextBox->GetEventHandler()->ProcessEvent(compareevent);  // �C�x���g������
}

//�݌v�g�p�o���l���擾�o���l���z�����ꍇ�̌x��
void Tab1Panel::OnCompareValues(wxCommandEvent& event) {
    long valueA = 0;
    long valueB = 0;
    // A���擾�o���l, B���݌v�g�p�o���l���擾
    valueA = getexpSpinCtrl->GetValue();
    allexpTextBox->GetValue().ToLong(&valueB);

    // B��A�����傫����΁A�x�����b�Z�[�W��\��
    if (valueB > valueA) {
        // StaticText��\��
        warningText->Show();
    }
    else {
        // A��B���傫���ꍇ��StaticText���\��
        warningText->Hide();
    }

    // �p�l�����ĕ`�悵��UI���X�V
    Layout();
}




void Tab1Panel::OnSpinCtrlChange(wxCommandEvent& event) {
    Updateexp(event);   // 1�ڂ̏���
    Updatelevel(event); // 2�ڂ̏���
	Updatehp(event); // 3�ڂ̏���
}

void Tab1Panel::abilityChange(wxCommandEvent& event) {
    Updatebonus(event);   // 1�ڂ̏���
    Updatehp(event); // 2�ڂ̏���
}


// json�t�@�C���ւ̏����o������

bool Tab1Panel::SaveToJson(const wxString& filename) {
    json root;

    // ������f�[�^
    root["name"] = std::string(nameTextBox->GetValue().ToUTF8().data()); // ���O
    root["PL"] = std::string(PLTextBox->GetValue().ToUTF8().data()); // PL��
    root["species"] = std::string(speciesTextBox->GetValue().ToUTF8().data()); // �푰
    root["age"] = wxAtoi(ageTextBox->GetValue()); // �N��
    root["sex"] = std::string(sexTextBox->GetValue().ToUTF8().data()); // ����
    root["spfeature"] = std::string(spfeatureTextBox->GetValue().ToUTF8().data()); // �푰����
    root["impurity"] = wxAtoi(ImpurityTextBox->GetValue()); // �q��
    root["birth"] = std::string(birthTextBox->GetValue().ToUTF8().data()); // ���܂�
    root["faith"] = std::string(faithTextBox->GetValue().ToUTF8().data()); // �M��
    root["rank"] = std::string(rankTextBox->GetValue().ToUTF8().data()); // �`���҃����N

    // �S�A�Z�A��
    for (int i = 0; i < 3; ++i) {
        root["born"][i] = wxAtoi(bornTextBoxes[i]->GetValue());  // ���l�Ƃ��ĕۑ�
    }

    // A�`F�A�����A��p�`���_�A�����A�{�[�i�X
    for (int i = 0; i < 6; ++i) {
        root["ability"][i] = wxAtoi(abilityTextBoxes[i]->GetValue());  // ���l�Ƃ��ĕۑ�
        root["growth"][i] = growthSpinCtrl[i]->GetValue();
        root["attribute"][i] = wxAtoi(attributeTextBoxes[i]->GetValue());
        root["increase"][i] = increaseSpinCtrl[i]->GetValue();
        root["bonus"][i] = wxAtoi(bonusTextBoxes[i]->GetValue());
    }

    // HP�AMP�A��R�́A�����́A�ǉ�D�A���́A�h��_
    for (int i = 0; i < 4; ++i) {
        root["hp"][i] = wxAtoi(hpTextBoxes[i]->GetValue());  // ���l�Ƃ��ĕۑ�
        root["hit"][i] = hitSpinCtrl[i]->GetValue();
    }

    // ����p�b�P�[�W
    for (int i = 0; i < 7; ++i) {
        root["pack"][i] = packSpinCtrl[i]->GetValue();
    }

    root["move"] = wxAtoi(moveTextBox->GetValue()); // �ړ���
    root["fullmove"] = wxAtoi(fullmoveTextBox->GetValue()); // �S�͈ړ�
    root["restmove"] = wxAtoi(restmoveTextBox->GetValue()); // �����ړ�

    // �Z�\�A�Z�\�e�[�u��A,B�A�Z�\���x���A�g�p�o���l
    for (int i = 0; i < 6; ++i) {
        root["tech"][i] = std::string(techTextBoxes[i]->GetValue().ToUTF8().data());
        root["table"][i] = tableradioA[i]->GetValue() ? "A" : "B"; // true�Ȃ�A�Afalse�Ȃ�B�Ƃ��������ŕۑ�
        root["techlevel"][i] = techlevelSpinCtrl[i]->GetValue();
        root["exp"][i] = wxAtoi(expTextBoxes[i]->GetValue());
    }

    root["level"] = wxAtoi(levelTextBox->GetValue()); // �`���҃��x��
    root["getexp"] = getexpSpinCtrl->GetValue(); // �擾�o���l
    root["allexp"] = wxAtoi(allexpTextBox->GetValue()); // ���v�g�p�o���l

    // �퓬���Z�A���ʊT�v
    for (int i = 0; i < 8; ++i) {
        root["talent"][i] = std::string(talentTextBoxes[i]->GetValue().ToUTF8().data());
        root["abst"][i] = std::string(abstTextBoxes[i]->GetValue().ToUTF8().data());
    }

    root["freetalent"] = std::string(freetalentTextBox->GetValue().ToUTF8().data()); // �����K�����Z
    root["freeabst"] = std::string(freeabstTextBox->GetValue().ToUTF8().data()); // �����K������
    root["magictalent"] = std::string(magictalentTextBox->GetValue().ToUTF8().data()); // �悭�g�����@�Ȃ�
    root["magicabst"] = std::string(magicabstTextBox->GetValue().ToUTF8().data()); // MP�A���ʊT�v
    root["battle"] = std::string(battleTextBox->GetValue().ToUTF8().data()); // �퓬���ʃ���
    root["other"] = std::string(otherTextBox->GetValue().ToUTF8().data()); // �o���A���̑�����

    // �g�p����A��b�A�Ǖ�
    for (int i = 0; i < 10; ++i) {
        root["lang"][i] = std::string(langTextBoxes[i]->GetValue().ToUTF8().data());
        root["speak"][i] = speakcheckBoxes[i]->GetValue();  // true �܂��� false ��ۑ�
        root["read"][i] = readcheckBoxes[i]->GetValue();    // true �܂��� false ��ۑ�
    }

    // ���햼
    for (int i = 0; i < 3; ++i) {
        root["weaponname"][i] = std::string(weaponnameTextBoxes[i]->GetValue().ToUTF8().data());  // ���l�Ƃ��ĕۑ�
    }

    // �p�@�A�K�؁A�����́A�З́AC�l�A�ǉ�D
    for (int i = 0; i < 6; ++i) {
        root["weaponuse"][i] = std::string(weaponuseTextBoxes[i]->GetValue().ToUTF8().data());
        root["weaponstr"][i] = wxAtoi(weaponstrTextBoxes[i]->GetValue());// ���l�Ƃ��ĕۑ�
        root["weaponhit"][i] = wxAtoi(weaponhitTextBoxes[i]->GetValue());
        root["weapondm"][i] = wxAtoi(weapondmTextBoxes[i]->GetValue());
        root["weaponcrit"][i] = wxAtoi(weaponcritTextBoxes[i]->GetValue());
        root["weaponadd"][i] = wxAtoi(weaponaddTextBoxes[i]->GetValue());
    }

    // ���l�A�h��A�K�؁A����C���A�h��_�A���l
    for (int i = 0; i < 3; ++i) {
        root["weapomother"][i] = std::string(weaponotherTextBoxes[i]->GetValue().ToUTF8().data());
        root["armorname"][i] = std::string(armornameTextBoxes[i]->GetValue().ToUTF8().data());
        root["armorstr"][i] = wxAtoi(armorstrTextBoxes[i]->GetValue());
        root["armorevade"][i] = wxAtoi(armorevadeTextBoxes[i]->GetValue());
        root["armorblock"][i] = wxAtoi(armorblockTextBoxes[i]->GetValue());
        root["armorother"][i] = std::string(armorotherTextBoxes[i]->GetValue().ToUTF8().data());
    }

    root["poket"] = poketSpinCtrl->GetValue(); // ������
    root["debt"] = wxAtoi(debtTextBox->GetValue()); // �a��/�؋�
    root["glory"] = glorySpinCtrl->GetValue(); // ���_�_

    for (int i = 0; i < 12; ++i) {
        root["aces"][i] = std::string(acesTextBoxes[i]->GetValue().ToUTF8().data()); // �����i
        root["aces2"][i] = std::string(acesTextBoxes2[i]->GetValue().ToUTF8().data()); // �����i����
    }
    root["bag"] = std::string(bagTextBox->GetValue().ToUTF8().data()); // �����A�C�e���Ȃ�

    // JSON�t�@�C���o��
    std::ofstream file(filename.ToStdString());
    if (!file) {
        wxLogError("Failed to open file: %s", filename);
        return false;
    }

    try {
        file << root.dump(4);
    }
    catch (const std::exception& e) {
        wxLogError("Failed to write JSON: %s", e.what());
        return false;
    }
    return true;
}


//// json�t�@�C���ǂݍ��݂̏���
bool Tab1Panel::LoadFromJson(const wxString& filename) {
    // JSON�t�@�C����ǂݍ���
    std::ifstream file(filename.ToStdString()); // �t�@�C���I�[�v��
    if (!file.is_open()) {
        return false; // �t�@�C�����J���Ȃ��ꍇ
    }

    nlohmann::json root;
    file >> root; // JSON�p�[�X
    file.close();

    // ������f�[�^�̃Z�b�g
    nameTextBox->SetValue(wxString::FromUTF8(root["name"].get<std::string>().c_str()));
    PLTextBox->SetValue(wxString::FromUTF8(root["PL"].get<std::string>().c_str()));
    speciesTextBox->SetValue(wxString::FromUTF8(root["species"].get<std::string>().c_str()));
    ageTextBox->SetValue(wxString::Format(wxT("%d"), root["age"].get<int>()));
    sexTextBox->SetValue(wxString::FromUTF8(root["sex"].get<std::string>().c_str()));
    spfeatureTextBox->SetValue(wxString::FromUTF8(root["spfeature"].get<std::string>().c_str()));
    ImpurityTextBox->SetValue(wxString::Format(wxT("%d"), root["impurity"].get<int>()));
    birthTextBox->SetValue(wxString::FromUTF8(root["birth"].get<std::string>().c_str()));
    faithTextBox->SetValue(wxString::FromUTF8(root["faith"].get<std::string>().c_str()));
    rankTextBox->SetValue(wxString::FromUTF8(root["rank"].get<std::string>().c_str()));

    // born[3] �̃Z�b�g�i�S�A�Z�A�́j
    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["born"][i].get<int>()));
    }

    // ability�Agrowth�Aattribute�Aincrease�Abonus �̃Z�b�g
    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["ability"][i].get<int>()));
        growthSpinCtrl[i]->SetValue(root["growth"][i].get<int>());
        attributeTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["attribute"][i].get<int>()));
        increaseSpinCtrl[i]->SetValue(root["increase"][i].get<int>());
        bonusTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["bonus"][i].get<int>()));
    }

    // hp�Ahit �̃Z�b�g
    for (int i = 0; i < 4; ++i) {
        hpTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["hp"][i].get<int>()));
        hitSpinCtrl[i]->SetValue(root["hit"][i].get<int>());
    }

    // pack �̃Z�b�g
    for (int i = 0; i < 6; ++i) {
        packSpinCtrl[i]->SetValue(root["pack"][i].get<int>());
    }

    // ���̑��̃f�[�^�i�ړ��́A�o���l�Ȃǁj
    moveTextBox->SetValue(wxString::Format(wxT("%d"), root["move"].get<int>()));
    fullmoveTextBox->SetValue(wxString::Format(wxT("%d"), root["fullmove"].get<int>()));
    restmoveTextBox->SetValue(wxString::Format(wxT("%d"), root["restmove"].get<int>()));

    levelTextBox->SetValue(wxString::Format(wxT("%d"), root["level"].get<int>()));
    getexpSpinCtrl->SetValue(root["getexp"].get<int>());
    allexpTextBox->SetValue(wxString::Format(wxT("%d"), root["allexp"].get<int>()));

    // �Z�\�Ȃ�
    for (int i = 0; i < 6; ++i) {
        techTextBoxes[i]->SetValue(wxString::FromUTF8(root["tech"][i].get<std::string>().c_str()));
        tableradioA[i]->SetValue(root["table"][i].get<std::string>() == "A");
        tableradioB[i]->SetValue(root["table"][i].get<std::string>() == "B");
                techlevelSpinCtrl[i]->SetValue(root["techlevel"][i].get<int>());
        expTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["exp"][i].get<int>()));
    }

    // �퓬���Z�A���ʊT�v
    for (int i = 0; i < 8; ++i) {
        talentTextBoxes[i]->SetValue(wxString::FromUTF8(root["talent"][i].get<std::string>().c_str()));
        abstTextBoxes[i]->SetValue(wxString::FromUTF8(root["abst"][i].get<std::string>().c_str()));
    }

    freetalentTextBox->SetValue(wxString::FromUTF8(root["freetalent"].get<std::string>().c_str()));
    freeabstTextBox->SetValue(wxString::FromUTF8(root["freeabst"].get<std::string>().c_str()));
    magictalentTextBox->SetValue(wxString::FromUTF8(root["magictalent"].get<std::string>().c_str()));
    magicabstTextBox->SetValue(wxString::FromUTF8(root["magicabst"].get<std::string>().c_str()));
    battleTextBox->SetValue(wxString::FromUTF8(root["battle"].get<std::string>().c_str()));
    otherTextBox->SetValue(wxString::FromUTF8(root["other"].get<std::string>().c_str()));

    // �g�p����A��b�A�Ǖ�
    for (int i = 0; i < 10; ++i) {
        langTextBoxes[i]->SetValue(wxString::FromUTF8(root["lang"][i].get<std::string>().c_str()));
        speakcheckBoxes[i]->SetValue(root["speak"][i].get<bool>());
        readcheckBoxes[i]->SetValue(root["read"][i].get<bool>());
    }

    // ���햼
    for (int i = 0; i < 3; ++i) {
        weaponnameTextBoxes[i]->SetValue(wxString::FromUTF8(root["weaponname"][i].get<std::string>().c_str()));
    }

    // ����p�@�A�K�؂Ȃ�
    for (int i = 0; i < 6; ++i) {
        weaponuseTextBoxes[i]->SetValue(wxString::FromUTF8(root["weaponuse"][i].get<std::string>().c_str()));
        weaponstrTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponstr"][i].get<int>()));
        weaponhitTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponhit"][i].get<int>()));
        weapondmTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weapondm"][i].get<int>()));
        weaponcritTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponcrit"][i].get<int>()));
        weaponaddTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponadd"][i].get<int>()));
    }

    // �h�
    for (int i = 0; i < 3; ++i) {
        weaponotherTextBoxes[i]->SetValue(wxString::FromUTF8(root["weapomother"][i].get<std::string>().c_str()));
        armornameTextBoxes[i]->SetValue(wxString::FromUTF8(root["armorname"][i].get<std::string>().c_str()));
        armorstrTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorstr"][i].get<int>()));
        armorevadeTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorevade"][i].get<int>()));
        armorblockTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorblock"][i].get<int>()));
        armorotherTextBoxes[i]->SetValue(wxString::FromUTF8(root["armorother"][i].get<std::string>().c_str()));
    }

    // �������A�؋��Ȃ�
    poketSpinCtrl->SetValue(root["poket"].get<int>());
    debtTextBox->SetValue(wxString::Format(wxT("%d"), root["debt"].get<int>()));
    glorySpinCtrl->SetValue(root["glory"].get<int>());
    // �����i�Ȃ�
    for (int i = 0; i < 12; ++i) {
        acesTextBoxes[i] -> SetValue(wxString::FromUTF8(root["aces"][i].get<std::string>().c_str()));
        acesTextBoxes2[i]->SetValue(wxString::FromUTF8(root["aces2"][i].get<std::string>().c_str()));
    }
    bagTextBox->SetValue(wxString::FromUTF8(root["bag"].get<std::string>().c_str()));

    return true;
}
