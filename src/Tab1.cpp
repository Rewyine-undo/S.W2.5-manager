#define WXJSON_USE_EXTERNAL
#include "Tab1.h"
#include "Tab2.h"
#include <algorithm>
#include <wx/filedlg.h>
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <numeric> 

using json = nlohmann::json;


Tab1Panel::Tab1Panel(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    {
    SetScrollRate(5, 5);  // スクロールの速度を設定

    sizer = new wxBoxSizer(wxVERTICAL);

    // JSONファイル読み込み用のUI
    wxStaticText* instruction = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("JSONファイルを読み込み、データを表示します。"));
    //wxTextCtrl* displayArea = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(550, 300), wxTE_MULTILINE | wxTE_READONLY);
    sizer->Add(instruction, 0, wxALL, 10);
    //sizer->Add(displayArea, 1, wxEXPAND | wxALL, 10);

    
    // 名前のテキストボックス
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("名前: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    nameTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(nameTextBox, 0, wxALIGN_CENTER| wxRIGHT, 3);
    nameSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("PL: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    PLTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(PLTextBox, 0, wxALIGN_CENTER);

    // 種族のテキストボックス
    wxBoxSizer* speciesSizer = new wxBoxSizer(wxHORIZONTAL);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("種族: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    speciesTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1));
    speciesTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    speciesSizer->Add(speciesTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("年齢: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ageTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(ageTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("性別: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    sexTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(sexTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    speciesSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("穢れ: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ImpurityTextBox = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(32, -1));
    speciesSizer->Add(ImpurityTextBox, 0, wxALIGN_CENTER);

    // 種族特徴のテキストボックス
    wxBoxSizer* spfeatureSizer = new wxBoxSizer(wxHORIZONTAL);
    spfeatureSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("種族特徴: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    spfeatureTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
    spfeatureSizer->Add(spfeatureTextBox, 0, wxALIGN_CENTER);
    

    // 信仰のテキストボックス
    wxBoxSizer* faithSizer = new wxBoxSizer(wxHORIZONTAL);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("生まれ: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    birthTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(65, -1));
    faithSizer->Add(birthTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("信仰: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    faithTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
    faithSizer->Add(faithTextBox, 0, wxALIGN_CENTER | wxRIGHT, 3);
    faithSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("ランク: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    rankTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("ー"), wxDefaultPosition, wxSize(80, -1));
    faithSizer->Add(rankTextBox, 0, wxALIGN_CENTER);

    sizer->Add(nameSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(speciesSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(spfeatureSizer, 0, wxEXPAND | wxALL, 3);
    sizer->Add(faithSizer, 0, wxEXPAND | wxALL, 3);

    wxStaticText* abi = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---能力値---"));
    sizer->Add(abi, 0, wxALL, 10);
    wxStaticText* comment = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("命中力などは技能、武具の中で最大値を参照"));
    comment->SetForegroundColour(*wxRED); // 赤色に設定
    sizer->Add(comment, 0, wxALIGN_RIGHT | wxBOTTOM, 5);

    // 親Sizerを横並びに設定
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main2Sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main1Sizer = new wxBoxSizer(wxVERTICAL);
    // bornTextBoxes（技・体・心）の初期化
    wxBoxSizer* bornSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bornLabels[3] = { wxString::FromUTF8("(技"), wxString::FromUTF8("(体"), wxString::FromUTF8("(心") };
    for (int i = 0; i < 3; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, bornLabels[i] + wxString::FromUTF8(" "));
        bornSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        bornTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, 68));
        bornSizer->Add(bornTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え

        // テキストを中央揃えに設定
        bornTextBoxes[i]->SetWindowStyleFlag(wxTE_CENTER);
        bornTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    // abilityTextBoxes（A～F）の初期化
    wxBoxSizer* abilitySizer = new wxBoxSizer(wxVERTICAL);
    const wxString abilityLabels[6] = { wxString::FromUTF8("+A"), wxString::FromUTF8("+B"), wxString::FromUTF8("+C"), wxString::FromUTF8("+D"), wxString::FromUTF8("+E"), wxString::FromUTF8("+F") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, abilityLabels[i] + wxString::FromUTF8(" "));
        abilitySizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        abilityTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        abilitySizer->Add(abilityTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        abilityTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    //成長値の項目
    wxBoxSizer* growthSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("+成長)"));
        growthSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // スピンボタンを追加
        growthSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        growthSizer->Add(growthSpinCtrl[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        growthSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::UpdateAttribute, this);
    }

    // attributeTextBoxes（器用～精神）の初期化
    wxBoxSizer* attributeSizer = new wxBoxSizer(wxVERTICAL);
    const wxString attributeLabels[6] = { wxString::FromUTF8("=(器用度"), wxString::FromUTF8("=(敏捷度"), wxString::FromUTF8("=(筋力"), wxString::FromUTF8("=(生命力"), wxString::FromUTF8("=(知力"), wxString::FromUTF8("=(精神力") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, attributeLabels[i] + wxString::FromUTF8(" "));
        attributeSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        attributeTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
        attributeSizer->Add(attributeTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        attributeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::abilityChange, this);
    }
    
    //増強値の項目
    wxBoxSizer* increaseSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("+増強)"));
        increaseSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // スピンボタンを追加
        increaseSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        increaseSizer->Add(increaseSpinCtrl[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        increaseSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::Updatebonus, this);
    }

    // bonusTextBoxes（器用～精神ボーナス）の初期化
    wxBoxSizer* bonusSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bonusLabels[6] = { wxString::FromUTF8("/6→器用度B"), wxString::FromUTF8("/6→敏捷度B"), wxString::FromUTF8("/6→筋力B"), wxString::FromUTF8("/6→生命力B"), wxString::FromUTF8("/6→知力B"), wxString::FromUTF8("/6→精神力B") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, bonusLabels[i] + wxString::FromUTF8(" "));
        bonusSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        bonusTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(75, -1));
        bonusSizer->Add(bonusTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        bonusTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    
    // HP,MP,抵抗力の初期化
    wxBoxSizer* hpSizer = new wxBoxSizer(wxVERTICAL);
    const wxString hpLabels[4] = { wxString::FromUTF8("HP"), wxString::FromUTF8("MP"), wxString::FromUTF8("生命抵抗力"), wxString::FromUTF8("精神抵抗力")};
    for (int i = 0; i < 4; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, hpLabels[i] + wxString::FromUTF8(" "));
        hpSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        hpTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
        hpSizer->Add(hpTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
     }
    const wxString hitLabels[4] = {wxString::FromUTF8("命中力"), wxString::FromUTF8("追加D"), wxString::FromUTF8("回避力"), wxString::FromUTF8("防護点") };
    for (int i = 0; i < 4; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, hitLabels[i] + wxString::FromUTF8(" "));
        hpSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        hitSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        hpSizer->Add(hitSpinCtrl[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }


    // HP,MP,判定パッケージの初期化
    wxBoxSizer* packSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* packlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("判定パッケージ: "));
    packSizer->Add(packlabel, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString packLabels[7] = {wxString::FromUTF8("技巧") , wxString::FromUTF8("運動") , wxString::FromUTF8("観察") , wxString::FromUTF8("知識"), wxString::FromUTF8("魔物知識"), wxString::FromUTF8("先制力"), wxString::FromUTF8("魔力")};
    for (int i = 0; i < 7; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(this, wxID_ANY, packLabels[i] + wxString::FromUTF8(" "));
        packSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        packSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        packSizer->Add(packSpinCtrl[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え

    }


    // 横並びにする
    main2Sizer->Add(bornSizer, 0, wxALIGN_CENTER);   
    main2Sizer->Add(abilitySizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(growthSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(attributeSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(increaseSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(bonusSizer, 0, wxALIGN_CENTER | wxLEFT, 15);

    // 移動力
    wxBoxSizer* moveSizer = new wxBoxSizer(wxHORIZONTAL);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("移動力: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    moveTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(moveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("m")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("*3= 全力移動: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    fullmoveTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(fullmoveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("m")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("制限移動: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    restmoveTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("3"), wxDefaultPosition, wxSize(50, -1));
    moveSizer->Add(restmoveTextBox, 0, wxALIGN_CENTER | wxRIGHT, 0);
    moveSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("m")), 0, wxALIGN_CENTER_VERTICAL);


    main1Sizer->Add(main2Sizer, 0, wxALIGN_CENTER| wxBOTTOM, 25);
    main1Sizer->Add(moveSizer, 0, wxALIGN_CENTER);


    mainSizer->Add(main1Sizer, 0, wxALIGN_TOP);
    mainSizer->Add(hpSizer, 0, wxALIGN_TOP | wxLEFT, 30);
    mainSizer->Add(packSizer, 0, wxALIGN_CENTER | wxLEFT, 10);

    // mainSizerを親Sizerに追加
    sizer->Add(mainSizer, 0, wxEXPAND | wxALL, 2);




    wxStaticText* tec = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---技能・特技---"));
    sizer->Add(tec, 0, wxALL, 10);


    // 親Sizerを横並びに設定
    wxBoxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* ssubSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sub1Sizer = new wxBoxSizer(wxHORIZONTAL);

    // 技能の初期化
    wxBoxSizer* techSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* techlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("技能 "));
    techSizer->Add(techlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        techTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        techSizer->Add(techTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        techTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
     }

    // 技能テーブルの初期化
    wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* tablelabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("テーブル "));
    tableSizer->Add(tablelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // ラジオボタンを追加
        wxBoxSizer* subtableSizer = new wxBoxSizer(wxHORIZONTAL);
        tableradioA[i] = new wxRadioButton(this, wxID_ANY, wxString::FromUTF8("A"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        tableradioB[i] = new wxRadioButton(this, wxID_ANY, wxString::FromUTF8("B"));
        
        subtableSizer->Add(tableradioA[i], 0, wxALIGN_CENTER );
        subtableSizer->Add(tableradioB[i], 0, wxALIGN_CENTER );
        tableSizer->Add(subtableSizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 4);
        // イベントを登録
        tableradioA[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
        tableradioB[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
    }
    

    // 技能レベルの初期化
    wxBoxSizer* techlevelSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* techlevellabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("レベル "));
    techlevelSizer->Add(techlevellabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // スピンボタンを追加
        techlevelSpinCtrl[i] = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 15, 0);
        techlevelSizer->Add(techlevelSpinCtrl[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        techlevelSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnSpinCtrlChange, this);
     }

    // 必要経験値の初期化
    wxBoxSizer* expSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* explabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("使用経験値 "));
    expSizer->Add(explabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        expTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
        expSizer->Add(expTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        expTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updateallexp, this);
    }

    // 横並びにする
    sub1Sizer->Add(techSizer, 0, wxALIGN_CENTER);   
    sub1Sizer->Add(tableSizer, 0, wxALIGN_CENTER | wxRIGHT|wxLEFT, 3);
    sub1Sizer->Add(techlevelSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    sub1Sizer->Add(expSizer, 0, wxALIGN_CENTER | wxLEFT, 2);

    // 冒険者レベル、取得経験値、合計使用経験値
    wxBoxSizer* allexpSizer = new wxBoxSizer(wxHORIZONTAL);
       
    allexpSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("取得経験値: ")), 0, wxALIGN_CENTER_VERTICAL);
    getexpSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000000, 0);
    getexpSpinCtrl->SetIncrement(50); // 変化幅を50に設定
    getexpSpinCtrl->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    getexpSpinCtrl->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(getexpSpinCtrl, 0, wxALIGN_CENTER | wxRIGHT, 6);
    
    allexpSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("合計使用経験値: ")), 0, wxALIGN_CENTER_VERTICAL );
    allexpTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
    allexpTextBox->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(allexpTextBox, 0, wxALIGN_CENTER);

    // 冒険者レベル、取得経験値、合計使用経験値
    wxBoxSizer* levelSizer = new wxBoxSizer(wxHORIZONTAL);
    levelSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("冒険者レベル: ")), 0, wxALIGN_CENTER_VERTICAL );
    levelTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
    levelTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    levelSizer->Add(levelTextBox, 0, wxALIGN_CENTER);

    warningText = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("取得経験値を超過しています！"), wxPoint(10, 10));
    warningText->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    warningText->SetForegroundColour(*wxRED); // 赤色に設定
    warningText->Hide(); // 初期状態では非表示にする
    levelSizer->Add(warningText, 0, wxALIGN_CENTER| wxLEFT, 10);



    ssubSizer->Add(sub1Sizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(allexpSizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(levelSizer, 0, wxALIGN_LEFT);


    wxBoxSizer* sub2Sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* talentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText * talentlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("戦闘特技"));
    talentSizer->Add(talentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 8; ++i) {
        wxString talentText = wxString::Format(wxString::FromUTF8("%d: "), 2 * i + 1); // 数値を文字列に変換
        // テキストボックスを追加
        talentTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, talentText, wxDefaultPosition, wxSize(120, -1));
        talentSizer->Add(talentTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        talentTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* abstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* abstlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("効果概要"));
    abstSizer->Add(abstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 8; ++i) {
        // テキストボックスを追加
        abstTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        abstSizer->Add(abstTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    // 横並びにする
    sub2Sizer->Add(talentSizer, 0, wxALIGN_CENTER);
    sub2Sizer->Add(abstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);

    //sub1Sizerとsub2SeizerをsubSeizerにまとめる
    subSizer->Add(ssubSizer, 0, wxALIGN_TOP);
    subSizer->Add(sub2Sizer, 0, wxALIGN_TOP | wxLEFT, 20);

    // subSizerを親Sizerに追加
    sizer->Add(subSizer, 0, wxEXPAND | wxLEFT, 3);


    // 記述欄の設定
    freespaceSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* freeSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* freetalentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* freetalentlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("自動取得など"));
    freetalentSizer->Add(freetalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    freetalentTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freetalentTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    freetalentSizer->Add(freetalentTextBox, 0, wxALIGN_LEFT | wxLEFT, 3);

    wxBoxSizer* freeabstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* freeabstlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("効果概要"));
    freeabstSizer->Add(freeabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    freeabstTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freeabstSizer->Add(freeabstTextBox, 0, wxALIGN_LEFT);

    freeSizer->Add(freetalentSizer, 0, wxALIGN_CENTER);
    freeSizer->Add(freeabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    wxBoxSizer* magicSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* magictalentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* magictalentlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("よく使う魔法など"));
    magictalentSizer->Add(magictalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    magictalentTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magictalentSizer->Add(magictalentTextBox, 0, wxALIGN_LEFT);

    wxBoxSizer* magicabstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* magicabstlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("MP, 効果概要"));
    magicabstSizer->Add(magicabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    magicabstTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magicabstSizer->Add(magicabstTextBox, 0, wxALIGN_LEFT);

    magicSizer->Add(magictalentSizer, 0, wxALIGN_CENTER);
    magicSizer->Add(magicabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    wxBoxSizer* battleSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* battlelabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("戦闘関連メモ"));
    wxStaticText* battlelabel2 = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("(受けた効果など)"));
    battleSizer->Add(battlelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    battleSizer->Add(battlelabel2, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    battleTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 131), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    battleSizer->Add(battleTextBox, 0, wxALIGN_LEFT);

        
    freespaceSizer->Add(freeSizer, 0, wxALIGN_CENTER);
    freespaceSizer->Add(magicSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    freespaceSizer->Add(battleSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    
    

    sizer->Add(freespaceSizer, 0, wxALIGN_LEFT | wxTOP, 15);

    placeholderSizer = new wxBoxSizer(wxVERTICAL); // その他技能の入る場所

    otherskillSizer = new wxBoxSizer(wxHORIZONTAL);
    placeholderSizer->Add(otherskillSizer, 0, wxALIGN_LEFT);
    sizer->Add(placeholderSizer, 0, wxALL, 5);


    // 親Sizerを横並びに設定
    trioSizer = new wxBoxSizer(wxHORIZONTAL);

    // 言語の初期化
    wxBoxSizer* langSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* langlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("言語"));
    langSizer->Add(langlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString langtext[10] = { wxString::FromUTF8("交易共通語"), wxString::FromUTF8("汎用蛮族語"), wxString::FromUTF8("地方語(ブルライト)"), wxString::FromUTF8("種族言語( )"), wxString::FromUTF8("魔法文明語"), wxString::FromUTF8("魔動機文明語"), wxString::FromUTF8("妖精語"), wxString::FromUTF8("神紀文明語"), wxString::FromUTF8("魔神語"), ""};
    for (int i = 0; i < 10; ++i) {
        // テキストボックスを追加
        langTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, langtext[i], wxDefaultPosition, wxSize(110, -1));
        langSizer->Add(langTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    // 会話の初期化
    wxBoxSizer* speakSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* speaklabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("会話"));
    speakSizer->Add(speaklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 10; ++i) {
        // チェックボックスを追加
        speakcheckBoxes[i] = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        speakSizer->Add(speakcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 4);
    }
    speakcheckBoxes[0]->SetValue(true);

    // 読文の初期化
    wxBoxSizer* readSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* readlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("読文"));
    readSizer->Add(readlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 10; ++i) {
        // チェックボックスを追加
        readcheckBoxes[i] = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        readSizer->Add(readcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 4);
    }
    readcheckBoxes[0]->SetValue(true);

    //sub1Sizerとsub2SeizerをsubSeizerにまとめる
    trioSizer->Add(langSizer, 0, wxALIGN_TOP | wxLEFT, 2);
    trioSizer->Add(speakSizer, 0, wxALIGN_TOP | wxLEFT, 1);
    trioSizer->Add(readSizer, 0, wxALIGN_TOP | wxLEFT, 1);

    wxBoxSizer* otherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* otherlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("経歴、その他メモなど"));
    otherSizer->Add(otherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    otherTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(360, 235), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    otherSizer->Add(otherTextBox, 0, wxALIGN_LEFT);
    
    // weaponSizerをtrioSizerに追加
    trioSizer->Add(otherSizer, 0, wxALIGN_TOP | wxLEFT, 5);

    // subSizerを親Sizerに追加
    sizer->Add(trioSizer, 0, wxEXPAND | wxTOP, 15);


    wxStaticText* item = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---武具・所持品---"));
    sizer->Add(item, 0, wxALL, 10);

    // 所持品欄の初期化
    wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

    // 武器欄の初期化
    wxBoxSizer* weaponSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* weaponnameSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponnamelabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("武器"));
    weaponnameSizer->Add(weaponnamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        weaponnameTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, 46));
        weaponnameSizer->Add(weaponnameTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponuseSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponuselabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("用法"));
    weaponuseSizer->Add(weaponuselabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponuseTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weaponuseSizer->Add(weaponuseTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponstrSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponstrlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("必筋"));
    weaponstrSizer->Add(weaponstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponstrTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(40, -1));
        weaponstrSizer->Add(weaponstrTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponhitSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponhitlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("命中力"));
    weaponhitSizer->Add(weaponhitlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponhitTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        weaponhitSizer->Add(weaponhitTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        weaponhitTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* weapondmSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weapondmlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("威力"));
    weapondmSizer->Add(weapondmlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weapondmTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weapondmSizer->Add(weapondmTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }


    wxBoxSizer* weaponcritSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponcritlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("C値"));
    weaponcritSizer->Add(weaponcritlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponcritTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("11"), wxDefaultPosition, wxSize(50, -1));
        weaponcritSizer->Add(weaponcritTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }


    wxBoxSizer* weaponaddSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponaddlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("追加D"));
    weaponaddSizer->Add(weaponaddlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponaddTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
        weaponaddSizer->Add(weaponaddTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        weaponaddTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }


    wxBoxSizer* weaponotherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponotherlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("備考"));
    weaponotherSizer->Add(weaponotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        weaponotherTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(140, 46), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
        weaponotherSizer->Add(weaponotherTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }
    
    //weapon系列をまとめる
    weaponSizer->Add(weaponnameSizer, 0, wxALIGN_CENTER);
    weaponSizer->Add(weaponuseSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponhitSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weapondmSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponcritSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponaddSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    // 防具欄と所持金欄をあわせる
    wxBoxSizer* armorSizer = new wxBoxSizer(wxHORIZONTAL);

    // 防具欄の初期化
    wxBoxSizer* subarmorSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* armornameSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armornamelabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("防具"));
    armornameSizer->Add(armornamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString armortext[3] = { wxString::FromUTF8("鎧"), wxString::FromUTF8("盾"), wxString::FromUTF8("その他") };
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armornameTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, armortext[i] + wxString::FromUTF8(":"), wxDefaultPosition, wxSize(110, -1));
        armornameSizer->Add(armornameTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* armorstrSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorstrlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("必筋"));
    armorstrSizer->Add(armorstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorstrTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        armorstrSizer->Add(armorstrTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* armorevadeSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorevadelabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("回避修正"));
    armorevadeSizer->Add(armorevadelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorevadeTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
        armorevadeSizer->Add(armorevadeTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        armorevadeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* armorblockSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorblocklabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("防護点"));
    armorblockSizer->Add(armorblocklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorblockTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        armorblockSizer->Add(armorblockTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        armorblockTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* armorotherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorotherlabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("備考"));
    armorotherSizer->Add(armorotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorotherTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        armorotherSizer->Add(armorotherTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    //armor系列をまとめる
    subarmorSizer->Add(armornameSizer, 0, wxALIGN_CENTER);
    subarmorSizer->Add(armorstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorevadeSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorblockSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);



    // 金銭のテキストボックス
    wxBoxSizer* moneySizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* poketSizer = new wxBoxSizer(wxHORIZONTAL);
    poketSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("所持金: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    poketSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000, 0);
    poketSizer->Add(poketSpinCtrl, 0, wxALIGN_CENTER);
    poketSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("G")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* debtSizer = new wxBoxSizer(wxHORIZONTAL);
    debtSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("預金/借金: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    debtTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
    debtSizer->Add(debtTextBox, 0, wxALIGN_CENTER);
    debtSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("G")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* glorySizer = new wxBoxSizer(wxHORIZONTAL);
    glorySizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("名誉点: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    glorySpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000, 0);
    glorySizer->Add(glorySpinCtrl, 0, wxALIGN_CENTER);
    glorySizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("点")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

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
    // ラベルを追加
    wxStaticText* aceslabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("各部位の装飾品"));
    acesSizer1->Add(aceslabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString acesLabels[12] = { wxString::FromUTF8("頭"), wxString::FromUTF8("顔"), wxString::FromUTF8("耳"), wxString::FromUTF8("首"), wxString::FromUTF8("背中"), wxString::FromUTF8("右手"), wxString::FromUTF8("左手"), wxString::FromUTF8("腰"), wxString::FromUTF8("足"), wxString::FromUTF8("その他"), wxString::FromUTF8("その他2"), wxString::FromUTF8("その他3") };
    for (int i = 0; i < 12; ++i) { //装飾品名
        acesTextBoxes[i] = new wxTextCtrl(this, wxID_ANY, acesLabels[i] + wxString::FromUTF8(": "), wxDefaultPosition, wxSize(150, -1));
        acesSizer1->Add(acesTextBoxes[i], 0, wxALIGN_LEFT);
    }
    acesTextBoxes[10]->Hide();
    acesTextBoxes[11]->Hide();
    
    // ラベルを追加
    wxStaticText* aceslabel2 = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("効果概要"));
    acesSizer2->Add(aceslabel2, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 12; ++i) { // 装飾品の効果
        acesTextBoxes2[i] = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(175, -1));
        acesSizer2->Add(acesTextBoxes2[i], 0, wxALIGN_LEFT);
        
    }
    acesTextBoxes2[10]->Hide();
    acesTextBoxes2[11]->Hide();
    equipSizer->Add(acesSizer1, 0, wxALIGN_LEFT);
    equipSizer->Add(acesSizer2, 0, wxALIGN_LEFT);
    


    wxBoxSizer* bagSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* baglabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("所持アイテムなど"));
    bagSizer->Add(baglabel, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ラベルの下にスペースを追加
    bagTextBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(250, 300), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    bagSizer->Add(bagTextBox, 0, wxALIGN_LEFT);

    equipSizer->Add(bagSizer, 0, wxLEFT, 5);
    sizer->Add(equipSizer, 0, wxEXPAND);




    SetSizer(sizer);
    Layout();
    FitInside();
}


void Tab1Panel::SetCharacterData(const wxString& name, const wxString& species, const wxString& feature, const std::vector<int>& data){
    if (data.size() < 5) return;  // データ数が足りなければ何もしない

    nameTextBox->SetValue(name);
    speciesTextBox->SetValue(species);
    spfeatureTextBox->SetValue(feature);
    ImpurityTextBox->SetValue(wxString::Format("%d", data[9]));

    // 技・体・心
    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->SetValue(wxString::Format("%d", data[i]));
    }
    // A～F
    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->SetValue(wxString::Format("%d", data[3 + i]));
    }
    //// 器用・敏捷・筋力・生命・知力・精神
    //for (int i = 0; i < 6; ++i) {
    //    attributeTextBoxes[i]->SetValue(wxString::Format("%d", data[9 + i]));
    //}
}

void Tab1Panel::UpdateAttribute(wxCommandEvent& event) {
    long bornVal[3], abilityVal[6], growthVal[6], increaseVal[6]; // 取得する値を格納する配列

    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->GetValue().ToLong(&bornVal[i]);
    }

    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->GetValue().ToLong(&abilityVal[i]);
        growthVal[i] = growthSpinCtrl[i]->GetValue();
        increaseVal[i] = increaseSpinCtrl[i]->GetValue();
    }

    // 合計を 各attributeTextBoxes にセット
    attributeTextBoxes[0]->SetValue(wxString::Format("%ld", bornVal[0] + abilityVal[0] + growthVal[0]));
    attributeTextBoxes[1]->SetValue(wxString::Format("%ld", bornVal[0] + abilityVal[1] + growthVal[1]));
    attributeTextBoxes[2]->SetValue(wxString::Format("%ld", bornVal[1] + abilityVal[2] + growthVal[2]));
    attributeTextBoxes[3]->SetValue(wxString::Format("%ld", bornVal[1] + abilityVal[3] + growthVal[3]));
    attributeTextBoxes[4]->SetValue(wxString::Format("%ld", bornVal[2] + abilityVal[4] + growthVal[4]));
    attributeTextBoxes[5]->SetValue(wxString::Format("%ld", bornVal[2] + abilityVal[5] + growthVal[5]));

}

void Tab1Panel::Updatebonus(wxCommandEvent& event) {
    long attributeVal[6], increaseVal[6]; // 取得する値を格納する配列

    for (int i = 0; i < 6; ++i) {
        attributeTextBoxes[i]->GetValue().ToLong(&attributeVal[i]);
        increaseVal[i] = increaseSpinCtrl[i]->GetValue();
    }

    // ボーナス計算を各bonusTextBoxにセット
    for (int i = 0; i < 6; ++i) {
        bonusTextBoxes[i]->SetValue(wxString::Format("%ld", (attributeVal[i] + increaseVal[i]) / 6));
    }
}

void Tab1Panel::Updatelevel(wxCommandEvent& event) {
    long techlevel[6]; //取得する値を格納する配列
    int advlevel=0;
    for (int i = 0; i < 6; ++i) {
        techlevel[i] = techlevelSpinCtrl[i]->GetValue();
    }
    // 配列の中の最大値を取得
    advlevel = *std::max_element(techlevel, techlevel + 6);

    levelTextBox->SetValue(wxString::Format("%ld", advlevel));
}

void Tab1Panel::Updatehp(wxCommandEvent& event) {
    int lv = 0, aglity = 0, vital = 0, magic = 0, hit = 0, evade = 0, damage = 0, mplv=0, mlv = 0, armor = 0, tarmor = 0 ,weaponhit = 0, weapondamage = 0, armorevade = 0, tarmorevade = 0, tech = 0, mov = 0, sage = 0 ;
    long bonus[6], hitally[6], evadeally[6], damageally[6],  magically[6], weaponhitally[6], weapondamageally[6];
    std::fill(magically, magically + 6, 0), std::fill(hitally, hitally + 6, 0), std::fill(evadeally, evadeally + 6, 0), std::fill(damageally, damageally + 6, 0);
    bool enhancerAdded = false;  // 追加済みかどうかのフラグ

    levelTextBox->GetValue().ToInt(&lv);
    attributeTextBoxes[1]->GetValue().ToInt(&aglity);
    attributeTextBoxes[3]->GetValue().ToInt(&vital);
    attributeTextBoxes[5]->GetValue().ToInt(&magic);

    for (int i = 0; i < 6; ++i) {
		bonusTextBoxes[i]->GetValue().ToLong(&bonus[i]); // ボーナスを取得
    }

    int evadetrue = 0;
    wxBoxSizer* newSizer = new wxBoxSizer(wxHORIZONTAL); // lv習得技能の見出し
    wxBoxSizer* lvskillSizer1 = new wxBoxSizer(wxVERTICAL); // テキストボックス横並べ
    wxBoxSizer* lvskillSizer2 = new wxBoxSizer(wxVERTICAL); // テキストボックス横並べ

    for (int i = 0; i < 6; ++i) {
		weaponhitTextBoxes[i]->GetValue().ToLong(&weaponhitally[i]); // 武器の命中力を取得
        weaponaddTextBoxes[i]->GetValue().ToLong(&weapondamageally[i]); // 武器の追加Dを取得

        std::string className = techTextBoxes[i]->GetValue().ToStdString(); // 技能名を取得
        if (className.empty()) {
            //this->Layout();
            continue; // 技能名が空の場合はスキップ
        }

        else if (className == wxString::FromUTF8("ファイター") || className == wxString::FromUTF8("グラップラー") || className == wxString::FromUTF8("フェンサー")) {
            hitally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[0];
            evadeally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[1];
			damageally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[2];

		} // 戦士系技能 命中力、回避力、追加D

        else if (className == wxString::FromUTF8("シューター")) {
            hitally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[0];
            damageally[i] = techlevelSpinCtrl[i]->GetValue(); //筋力Bなし
            
            for (int i = 0; i < 8; ++i) { // 回避の判定
                std::string talentName = talentTextBoxes[i]->GetValue().ToStdString(); // 特技名を取得
                if (talentName.find(wxString::FromUTF8("射手の体術")) != std::string::npos){
					evadetrue = 1; // 射手の体術がある場合
                    break;
                }
            }
            if (evadetrue == 1) {
                evadeally[i] = techlevelSpinCtrl[i]->GetValue() + bonus[1];
            }
        } // シューター 命中力、回避力は特殊

        else if (className == wxString::FromUTF8("ソーサラー") || className == wxString::FromUTF8("コンジャラー") || className == wxString::FromUTF8("プリースト") || className == wxString::FromUTF8("マギテック") || className == wxString::FromUTF8("フェアリーテイマー")) {
            magically[i] = techlevelSpinCtrl[i]->GetValue();
        }// 魔法使い系技能

        else if (className == wxString::FromUTF8("スカウト") || className == wxString::FromUTF8("レンジャー")) {
            tech = techlevelSpinCtrl[i]->GetValue() + bonus[0]; // 技巧
            mov = techlevelSpinCtrl[i]->GetValue() + bonus[1]; // 運動、先制力
        }// 先制技能

        else if (className == wxString::FromUTF8("セージ") || className == wxString::FromUTF8("ライダー")) {
			sage = techlevelSpinCtrl[i]->GetValue() + bonus[4]; //観察、知識、魔物知識
        }// 魔物知識技能

        //else if (className == wxString::FromUTF8("エンハンサー") && !enhancerAdded) {
        //    // 全体をまとめる一時的なラッパーsizer
        //    wxBoxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);
        //    newSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("習得練技")), 0);

        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText1 = wxString::Format(wxString::FromUTF8("%d: "), j + 1); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        enhanceskillTextBoxes1[j] = new wxTextCtrl(this, wxID_ANY, otherskillText1, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(enhanceskillTextBoxes1[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //     }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText2 = wxString::Format(wxString::FromUTF8("%d: "), j + 9); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        enhanceskillTextBoxes2[j] = new wxTextCtrl(this, wxID_ANY, otherskillText2, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(enhanceskillTextBoxes2[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    // otherskillSizerごと追加
        //    wrapperSizer->Add(otherskillSizer, 0);
        //                 
        //    placeholderSizer->Add(wrapperSizer, 0);//最後に wrapper を placeholderSizer に追加！
        //    this->Layout();

        //    enhancerAdded = true;  // 二度と追加されないようにする
        //}

        //else if (className == wxString::FromUTF8("バード")) {
        //    placeholderSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("習得呪歌・終律")), 0, wxALL, 5);
        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText = wxString::Format(wxString::FromUTF8("%d: "), j + 1); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText = wxString::Format(wxString::FromUTF8("%d: "), j + 9); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    this->Layout();
        //}

        //else if (className == wxString::FromUTF8("ライダー")) {
        //    // 全体をまとめる一時的なラッパーsizer
        //    wxBoxSizer* wrapperSizer = new wxBoxSizer(wxVERTICAL);
        //    wrapperSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("習得騎芸")), 0);

        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText1 = wxString::Format(wxString::FromUTF8("%d: "), j + 1); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        enhanceskillTextBoxes1[j] = new wxTextCtrl(this, wxID_ANY, otherskillText1, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(enhanceskillTextBoxes1[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText2 = wxString::Format(wxString::FromUTF8("%d: "), j + 9); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        enhanceskillTextBoxes2[j] = new wxTextCtrl(this, wxID_ANY, otherskillText2, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(enhanceskillTextBoxes2[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    // otherskillSizerごと追加
        //    wrapperSizer->Add(otherskillSizer, 0);

        //    placeholderSizer->Add(wrapperSizer, 0);//最後に wrapper を placeholderSizer に追加！
        //    this->Layout();
        //}
         
        //else if (className == wxString::FromUTF8("アルケミスト")) {
        //    placeholderSizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("習得賦術")), 0, wxALL, 5);
        //    for (int j = 0; j < 8; ++j) {
        //        wxString otherskillText = wxString::Format(wxString::FromUTF8("%d: "), j + 1); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer1->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    for (int j = 0; j < 7; ++j) {
        //        wxString otherskillText = wxString::Format(wxString::FromUTF8("%d: "), j + 9); // 数値を文字列に変換
        //        //テキストボックスを追加
        //        otherskillTextBoxes[j] = new wxTextCtrl(this, wxID_ANY, otherskillText, wxDefaultPosition, wxSize(120, -1));
        //        lvskillSizer2->Add(otherskillTextBoxes[j], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        //    }
        //    newSizer->Add(lvskillSizer1, 0, wxALIGN_LEFT);
        //    newSizer->Add(lvskillSizer2, 0, wxALIGN_LEFT);
        //    otherskillSizer->Add(newSizer, 0, wxALIGN_LEFT);
        //    this->Layout();
        //}

        else {
            //this->Layout();
            continue; // 技能名が空の場合はスキップ
        }

    }
    // 配列の中の最大値を取得
	hit = *std::max_element(hitally, hitally + 6);
    evade = *std::max_element(evadeally, evadeally + 6);
    mplv = std::accumulate(magically, magically + 6, 0);
    mlv = *std::max_element(magically, magically + 6);
    damage = *std::max_element(damageally, damageally + 6);

    weaponhit = *std::max_element(weaponhitally, weaponhitally + 6);
    weapondamage = *std::max_element(weapondamageally, weapondamageally + 6);

    for (int i = 0; i < 3; ++i) {
		armorblockTextBoxes[i]->GetValue().ToInt(&armor); // 防具の防護点を取得
        tarmor += armor;
        armorevadeTextBoxes[i]->GetValue().ToInt(&armorevade); // 防具の回避修正を取得
		tarmorevade += armorevade;
    }

	std::string species = speciesTextBox->GetValue().ToStdString(); // 種族名を取得
    if (species == wxString::FromUTF8("リルドラケン")) {
        tarmor += 1;
	} // リルドラケンの種族特性により防護点+1
    else if (species == wxString::FromUTF8("レプラカーン")) {
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
    } // レプラカーンの種族特性によりlv6からその他の部位+1, lv11


    for (int i = 0; i < 8; ++i) { // 常時特技による能力値の修正
        std::string talentName = talentTextBoxes[i]->GetValue().ToStdString(); // 特技名を取得
        if (talentName.empty()) continue; // 技能名が空の場合はスキップ

        else if (talentName.find(wxString::FromUTF8("足さばき")) != std::string::npos) {
            restmoveTextBox->SetValue(wxString::Format(wxString::FromUTF8("10"))); // 足さばきがある場合
        }
        else if (talentName.find(wxString::FromUTF8("回避行動Ⅰ")) != std::string::npos) {
            evade += 1; // 回避行動Ⅰがある場合
        }
        else if (talentName.find(wxString::FromUTF8("回避行動Ⅱ")) != std::string::npos) {
            evade += 2; // 回避行動Ⅱがある場合 
        }
        else if (talentName.find(wxString::FromUTF8("頑強")) != std::string::npos) {
            vital += 15; // 頑強がある場合
        }
        else if (talentName.find(wxString::FromUTF8("超頑強")) != std::string::npos) {
            vital += 15; // 超頑強がある場合
        }
        else if (talentName.find(wxString::FromUTF8("武器習熟A")) != std::string::npos) {
            weapondamage += 1; // 武器習熟Aがある場合
        }
        else if (talentName.find(wxString::FromUTF8("武器習熟S")) != std::string::npos) {
            weapondamage += 2; // 武器習熟Sがある場合
        }
        else if (talentName.find(wxString::FromUTF8("防具習熟A")) != std::string::npos) {
            tarmor += 1; // 防具習熟Aがある場合
        }
        else if (talentName.find(wxString::FromUTF8("防具習熟S")) != std::string::npos) {
            tarmor += 2; // 防具習熟Sがある場合
        }
        else if (talentName.find(wxString::FromUTF8("魔力強化Ⅰ")) != std::string::npos) {
            bonus[4] += 1; // 魔力強化Ⅰがある場合
        }
        else if (talentName.find(wxString::FromUTF8("魔力強化Ⅱ")) != std::string::npos) {
            bonus[4] += 2; // 魔力強化Ⅱがある場合
        }
        else if (talentName.find(wxString::FromUTF8("命中強化Ⅰ")) != std::string::npos) {
            weaponhit += 1; // 命中強化Ⅰがある場合
        }
        else if (talentName.find(wxString::FromUTF8("命中強化Ⅱ")) != std::string::npos) {
            weaponhit += 2; // 命中強化Ⅱがある場合
        }
        else if (talentName.find(wxString::FromUTF8("キャパシティ")) != std::string::npos) {
             magic+= 15; // キャパシティがある場合
        }

    }
    
    std::string freetalentName = freetalentTextBox->GetValue().ToStdString(); // 自動取得の特技名を取得
    if (freetalentName.find(wxString::FromUTF8("タフネス")) != std::string::npos) {
		vital += 15; // タフネスがある場合
    }

    // 冒険者レベルで計算できる項目
    hpTextBoxes[0]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (3 * lv + vital))); // Hp
    hpTextBoxes[2]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (lv + bonus[3]))); // 生命抵抗力
    hpTextBoxes[3]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (lv + bonus[5]))); // 精神抵抗力

    // 魔法技能を仕様する項目
    if (species == wxString::FromUTF8("グラスランナー")) {
        hpTextBoxes[1]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (0)));  
    } // グラスランナーはMP0
    else {
        hpTextBoxes[1]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (3 * mplv + magic)));  // Mp
    }


    hitSpinCtrl[0]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (hit + weaponhit)));// 命中力
    hitSpinCtrl[1]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (damage + weapondamage)));// 追加D
    hitSpinCtrl[2]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (evade + tarmorevade)));// 回避力
	hitSpinCtrl[3]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (tarmor)));// 防護点

    packSpinCtrl[0]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (tech)));// 技巧
    packSpinCtrl[1]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (mov)));// 運動
    packSpinCtrl[2]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (sage)));// 観察
    packSpinCtrl[3]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (sage)));// 知識
    packSpinCtrl[4]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (sage)));// 魔物知識
    packSpinCtrl[5]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (mov)));// 先制力
	packSpinCtrl[6]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (mlv + bonus[4])));// 魔力
    
    
    moveTextBox->SetValue(wxString::Format(wxString::FromUTF8("%ld"), aglity)); // 移動力
    fullmoveTextBox->SetValue(wxString::Format(wxString::FromUTF8("%ld"), 3*aglity)); // 全力移動力
}



void Tab1Panel::Updateexp(wxCommandEvent& event) {
    long techlevelVal[6], tableVal[6]; // 取得する値を格納する配列

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

    // exp計算を各expTextBoxにセット
    for (int i = 0; i < 6; ++i) {
        expTextBoxes[i]->SetValue(wxString::Format(wxString::FromUTF8("%ld"), tableVal[i]));
    }
}

void Tab1Panel::Updateallexp(wxCommandEvent& event) {
    long expVal[6]; // 取得する値を格納する配列
    int allexp{};
    for (int i = 0; i < 6; ++i) {
        expTextBoxes[i]->GetValue().ToLong(&expVal[i]);
        allexp += expVal[i];
    }
    allexpTextBox->ChangeValue(wxString::Format(wxString::FromUTF8("%ld"), allexp));
    //allexpTextBox->Update(); // UIを更新する

    // 手動でイベントを発生させる
    wxCommandEvent compareevent(wxEVT_COMMAND_TEXT_UPDATED, allexpTextBox->GetId());
    compareevent.SetEventObject(allexpTextBox);  // イベントオブジェクトを設定
    allexpTextBox->GetEventHandler()->ProcessEvent(compareevent);  // イベントを処理
}

//累計使用経験値が取得経験値を越した場合の警告
void Tab1Panel::OnCompareValues(wxCommandEvent& event) {
    long valueA = 0;
    long valueB = 0;
    // A→取得経験値, B→累計使用経験値を取得
    valueA = getexpSpinCtrl->GetValue();
    allexpTextBox->GetValue().ToLong(&valueB);

    // BがAよりも大きければ、警告メッセージを表示
    if (valueB > valueA) {
        // StaticTextを表示
        warningText->Show();
    }
    else {
        // AがBより大きい場合はStaticTextを非表示
        warningText->Hide();
    }

    // パネルを再描画してUIを更新
    Layout();
}




void Tab1Panel::OnSpinCtrlChange(wxCommandEvent& event) {
    Updateexp(event);   // 1つ目の処理
    Updatelevel(event); // 2つ目の処理
	Updatehp(event); // 3つ目の処理
}

void Tab1Panel::abilityChange(wxCommandEvent& event) {
    Updatebonus(event);   // 1つ目の処理
    Updatehp(event); // 2つ目の処理
}


// jsonファイルへの書き出し処理

bool Tab1Panel::SaveToJson(const wxString& filename) {
    json root;

    // 文字列データ
    root["name"] = std::string(nameTextBox->GetValue().ToUTF8().data()); // 名前
    root["PL"] = std::string(PLTextBox->GetValue().ToUTF8().data()); // PL名
    root["species"] = std::string(speciesTextBox->GetValue().ToUTF8().data()); // 種族
    root["age"] = wxAtoi(ageTextBox->GetValue()); // 年齢
    root["sex"] = std::string(sexTextBox->GetValue().ToUTF8().data()); // 性別
    root["spfeature"] = std::string(spfeatureTextBox->GetValue().ToUTF8().data()); // 種族特性
    root["impurity"] = wxAtoi(ImpurityTextBox->GetValue()); // 穢れ
    root["birth"] = std::string(birthTextBox->GetValue().ToUTF8().data()); // 生まれ
    root["faith"] = std::string(faithTextBox->GetValue().ToUTF8().data()); // 信仰
    root["rank"] = std::string(rankTextBox->GetValue().ToUTF8().data()); // 冒険者ランク

    // 心、技、体
    for (int i = 0; i < 3; ++i) {
        root["born"][i] = wxAtoi(bornTextBoxes[i]->GetValue());  // 数値として保存
    }

    // A～F、成長、器用～精神、増強、ボーナス
    for (int i = 0; i < 6; ++i) {
        root["ability"][i] = wxAtoi(abilityTextBoxes[i]->GetValue());  // 数値として保存
        root["growth"][i] = growthSpinCtrl[i]->GetValue();
        root["attribute"][i] = wxAtoi(attributeTextBoxes[i]->GetValue());
        root["increase"][i] = increaseSpinCtrl[i]->GetValue();
        root["bonus"][i] = wxAtoi(bonusTextBoxes[i]->GetValue());
    }

    // HP、MP、抵抗力、命中力、追加D、回避力、防護点
    for (int i = 0; i < 4; ++i) {
        root["hp"][i] = wxAtoi(hpTextBoxes[i]->GetValue());  // 数値として保存
        root["hit"][i] = hitSpinCtrl[i]->GetValue();
    }

    // 判定パッケージ
    for (int i = 0; i < 7; ++i) {
        root["pack"][i] = packSpinCtrl[i]->GetValue();
    }

    root["move"] = wxAtoi(moveTextBox->GetValue()); // 移動力
    root["fullmove"] = wxAtoi(fullmoveTextBox->GetValue()); // 全力移動
    root["restmove"] = wxAtoi(restmoveTextBox->GetValue()); // 制限移動

    // 技能、技能テーブルA,B、技能レベル、使用経験値
    for (int i = 0; i < 6; ++i) {
        root["tech"][i] = std::string(techTextBoxes[i]->GetValue().ToUTF8().data());
        root["table"][i] = tableradioA[i]->GetValue() ? "A" : "B"; // trueならA、falseならBという文字で保存
        root["techlevel"][i] = techlevelSpinCtrl[i]->GetValue();
        root["exp"][i] = wxAtoi(expTextBoxes[i]->GetValue());
    }

    root["level"] = wxAtoi(levelTextBox->GetValue()); // 冒険者レベル
    root["getexp"] = getexpSpinCtrl->GetValue(); // 取得経験値
    root["allexp"] = wxAtoi(allexpTextBox->GetValue()); // 合計使用経験値

    // 戦闘特技、効果概要
    for (int i = 0; i < 8; ++i) {
        root["talent"][i] = std::string(talentTextBoxes[i]->GetValue().ToUTF8().data());
        root["abst"][i] = std::string(abstTextBoxes[i]->GetValue().ToUTF8().data());
    }

    root["freetalent"] = std::string(freetalentTextBox->GetValue().ToUTF8().data()); // 自動習得特技
    root["freeabst"] = std::string(freeabstTextBox->GetValue().ToUTF8().data()); // 自動習得効果
    root["magictalent"] = std::string(magictalentTextBox->GetValue().ToUTF8().data()); // よく使う魔法など
    root["magicabst"] = std::string(magicabstTextBox->GetValue().ToUTF8().data()); // MP、効果概要
    root["battle"] = std::string(battleTextBox->GetValue().ToUTF8().data()); // 戦闘効果メモ
    root["other"] = std::string(otherTextBox->GetValue().ToUTF8().data()); // 経歴、その他メモ

    // 使用言語、会話、読文
    for (int i = 0; i < 10; ++i) {
        root["lang"][i] = std::string(langTextBoxes[i]->GetValue().ToUTF8().data());
        root["speak"][i] = speakcheckBoxes[i]->GetValue();  // true または false を保存
        root["read"][i] = readcheckBoxes[i]->GetValue();    // true または false を保存
    }

    // 武器名
    for (int i = 0; i < 3; ++i) {
        root["weaponname"][i] = std::string(weaponnameTextBoxes[i]->GetValue().ToUTF8().data());  // 数値として保存
    }

    // 用法、必筋、命中力、威力、C値、追加D
    for (int i = 0; i < 6; ++i) {
        root["weaponuse"][i] = std::string(weaponuseTextBoxes[i]->GetValue().ToUTF8().data());
        root["weaponstr"][i] = wxAtoi(weaponstrTextBoxes[i]->GetValue());// 数値として保存
        root["weaponhit"][i] = wxAtoi(weaponhitTextBoxes[i]->GetValue());
        root["weapondm"][i] = wxAtoi(weapondmTextBoxes[i]->GetValue());
        root["weaponcrit"][i] = wxAtoi(weaponcritTextBoxes[i]->GetValue());
        root["weaponadd"][i] = wxAtoi(weaponaddTextBoxes[i]->GetValue());
    }

    // 備考、防具名、必筋、回避修正、防護点、備考
    for (int i = 0; i < 3; ++i) {
        root["weapomother"][i] = std::string(weaponotherTextBoxes[i]->GetValue().ToUTF8().data());
        root["armorname"][i] = std::string(armornameTextBoxes[i]->GetValue().ToUTF8().data());
        root["armorstr"][i] = wxAtoi(armorstrTextBoxes[i]->GetValue());
        root["armorevade"][i] = wxAtoi(armorevadeTextBoxes[i]->GetValue());
        root["armorblock"][i] = wxAtoi(armorblockTextBoxes[i]->GetValue());
        root["armorother"][i] = std::string(armorotherTextBoxes[i]->GetValue().ToUTF8().data());
    }

    root["poket"] = poketSpinCtrl->GetValue(); // 所持金
    root["debt"] = wxAtoi(debtTextBox->GetValue()); // 預金/借金
    root["glory"] = glorySpinCtrl->GetValue(); // 名誉点

    for (int i = 0; i < 12; ++i) {
        root["aces"][i] = std::string(acesTextBoxes[i]->GetValue().ToUTF8().data()); // 装飾品
        root["aces2"][i] = std::string(acesTextBoxes2[i]->GetValue().ToUTF8().data()); // 装飾品効果
    }
    root["bag"] = std::string(bagTextBox->GetValue().ToUTF8().data()); // 所持アイテムなど

    // JSONファイル出力
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


//// jsonファイル読み込みの処理
bool Tab1Panel::LoadFromJson(const wxString& filename) {
    // JSONファイルを読み込む
    std::ifstream file(filename.ToStdString()); // ファイルオープン
    if (!file.is_open()) {
        return false; // ファイルが開けない場合
    }

    nlohmann::json root;
    file >> root; // JSONパース
    file.close();

    // 文字列データのセット
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

    // born[3] のセット（心、技、体）
    for (int i = 0; i < 3; ++i) {
        bornTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["born"][i].get<int>()));
    }

    // ability、growth、attribute、increase、bonus のセット
    for (int i = 0; i < 6; ++i) {
        abilityTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["ability"][i].get<int>()));
        growthSpinCtrl[i]->SetValue(root["growth"][i].get<int>());
        attributeTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["attribute"][i].get<int>()));
        increaseSpinCtrl[i]->SetValue(root["increase"][i].get<int>());
        bonusTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["bonus"][i].get<int>()));
    }

    // hp、hit のセット
    for (int i = 0; i < 4; ++i) {
        hpTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["hp"][i].get<int>()));
        hitSpinCtrl[i]->SetValue(root["hit"][i].get<int>());
    }

    // pack のセット
    for (int i = 0; i < 6; ++i) {
        packSpinCtrl[i]->SetValue(root["pack"][i].get<int>());
    }

    // その他のデータ（移動力、経験値など）
    moveTextBox->SetValue(wxString::Format(wxT("%d"), root["move"].get<int>()));
    fullmoveTextBox->SetValue(wxString::Format(wxT("%d"), root["fullmove"].get<int>()));
    restmoveTextBox->SetValue(wxString::Format(wxT("%d"), root["restmove"].get<int>()));

    levelTextBox->SetValue(wxString::Format(wxT("%d"), root["level"].get<int>()));
    getexpSpinCtrl->SetValue(root["getexp"].get<int>());
    allexpTextBox->SetValue(wxString::Format(wxT("%d"), root["allexp"].get<int>()));

    // 技能など
    for (int i = 0; i < 6; ++i) {
        techTextBoxes[i]->SetValue(wxString::FromUTF8(root["tech"][i].get<std::string>().c_str()));
        tableradioA[i]->SetValue(root["table"][i].get<std::string>() == "A");
        tableradioB[i]->SetValue(root["table"][i].get<std::string>() == "B");
                techlevelSpinCtrl[i]->SetValue(root["techlevel"][i].get<int>());
        expTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["exp"][i].get<int>()));
    }

    // 戦闘特技、効果概要
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

    // 使用言語、会話、読文
    for (int i = 0; i < 10; ++i) {
        langTextBoxes[i]->SetValue(wxString::FromUTF8(root["lang"][i].get<std::string>().c_str()));
        speakcheckBoxes[i]->SetValue(root["speak"][i].get<bool>());
        readcheckBoxes[i]->SetValue(root["read"][i].get<bool>());
    }

    // 武器名
    for (int i = 0; i < 3; ++i) {
        weaponnameTextBoxes[i]->SetValue(wxString::FromUTF8(root["weaponname"][i].get<std::string>().c_str()));
    }

    // 武器用法、必筋など
    for (int i = 0; i < 6; ++i) {
        weaponuseTextBoxes[i]->SetValue(wxString::FromUTF8(root["weaponuse"][i].get<std::string>().c_str()));
        weaponstrTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponstr"][i].get<int>()));
        weaponhitTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponhit"][i].get<int>()));
        weapondmTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weapondm"][i].get<int>()));
        weaponcritTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponcrit"][i].get<int>()));
        weaponaddTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["weaponadd"][i].get<int>()));
    }

    // 防具名
    for (int i = 0; i < 3; ++i) {
        weaponotherTextBoxes[i]->SetValue(wxString::FromUTF8(root["weapomother"][i].get<std::string>().c_str()));
        armornameTextBoxes[i]->SetValue(wxString::FromUTF8(root["armorname"][i].get<std::string>().c_str()));
        armorstrTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorstr"][i].get<int>()));
        armorevadeTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorevade"][i].get<int>()));
        armorblockTextBoxes[i]->SetValue(wxString::Format(wxT("%d"), root["armorblock"][i].get<int>()));
        armorotherTextBoxes[i]->SetValue(wxString::FromUTF8(root["armorother"][i].get<std::string>().c_str()));
    }

    // 所持金、借金など
    poketSpinCtrl->SetValue(root["poket"].get<int>());
    debtTextBox->SetValue(wxString::Format(wxT("%d"), root["debt"].get<int>()));
    glorySpinCtrl->SetValue(root["glory"].get<int>());
    // 装飾品など
    for (int i = 0; i < 12; ++i) {
        acesTextBoxes[i] -> SetValue(wxString::FromUTF8(root["aces"][i].get<std::string>().c_str()));
        acesTextBoxes2[i]->SetValue(wxString::FromUTF8(root["aces2"][i].get<std::string>().c_str()));
    }
    bagTextBox->SetValue(wxString::FromUTF8(root["bag"].get<std::string>().c_str()));

    return true;
}
