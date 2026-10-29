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
#include <string>
#include <wx/scrolwin.h>
#include <wx/collpane.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
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

    wxBoxSizer* allPaneButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* expandAllButton = new wxButton(
        this,
        wxID_ANY,
        wxString::FromUTF8("すべて展開")
    );

    wxButton* collapseAllButton = new wxButton(
        this,
        wxID_ANY,
        wxString::FromUTF8("すべて収納")
    );

    expandAllButton->Bind(
        wxEVT_BUTTON,
        &Tab1Panel::OnExpandAll,
        this
    );

    collapseAllButton->Bind(
        wxEVT_BUTTON,
        &Tab1Panel::OnCollapseAll,
        this
    );

    allPaneButtonSizer->Add(expandAllButton, 0, wxRIGHT, 5);
    allPaneButtonSizer->Add(collapseAllButton, 0);

    sizer->Add(allPaneButtonSizer, 0, wxALL, 5);

    //sizer->Add(displayArea, 1, wxEXPAND | wxALL, 10);

    //名前〜能力値までの折りたたみ
    wxCollapsiblePane* basicInfoPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---キャラクター基本情報---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(basicInfoPane);

    // 折りたたみ内部
    wxWindow* basicInfoWindow = basicInfoPane->GetPane();

    wxBoxSizer* basicInfoSizer = new wxBoxSizer(wxVERTICAL);

    basicInfoWindow->SetSizer(basicInfoSizer);

    basicInfoPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    basicInfoPane->Expand();

    // 名前のテキストボックス
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("名前: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    nameTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(nameTextBox, 0, wxALIGN_CENTER| wxRIGHT, 5);
    nameSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("PL: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    PLTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    nameSizer->Add(PLTextBox, 0, wxALIGN_CENTER);

    // 種族のテキストボックス
    wxBoxSizer* speciesSizer = new wxBoxSizer(wxHORIZONTAL);
    speciesSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("種族: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    speciesTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1));
    speciesTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    speciesSizer->Add(speciesTextBox, 0, wxALIGN_CENTER | wxRIGHT, 8);
    speciesSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("年齢: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ageTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(ageTextBox, 0, wxALIGN_CENTER | wxRIGHT, 8);
    speciesSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("性別: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    sexTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
    speciesSizer->Add(sexTextBox, 0, wxALIGN_CENTER | wxRIGHT, 8);
    speciesSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("穢れ: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    ImpurityTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "0", wxDefaultPosition, wxSize(32, -1));
    speciesSizer->Add(ImpurityTextBox, 0, wxALIGN_CENTER);

    // 種族特徴のテキストボックス
    wxBoxSizer* spfeatureSizer = new wxBoxSizer(wxHORIZONTAL);
    spfeatureSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("種族特徴: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    spfeatureTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(250, -1));
    spfeatureSizer->Add(spfeatureTextBox, 0, wxALIGN_CENTER);
    

    // 信仰のテキストボックス
    wxBoxSizer* faithSizer = new wxBoxSizer(wxHORIZONTAL);
    faithSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("生まれ: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    birthTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(65, -1));
    faithSizer->Add(birthTextBox, 0, wxALIGN_CENTER | wxRIGHT, 8);
    faithSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("信仰: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    faithTextBox = new wxTextCtrl(basicInfoWindow, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
    faithSizer->Add(faithTextBox, 0, wxALIGN_CENTER | wxRIGHT, 8);
    faithSizer->Add(new wxStaticText(basicInfoWindow, wxID_ANY, wxString::FromUTF8("ランク: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    // ランク選択
    wxArrayString rankItems;
    rankItems.Add(wxString::FromUTF8("--"));
    rankItems.Add(wxString::FromUTF8("レイピア"));
    rankItems.Add(wxString::FromUTF8("ブロードソード"));
    rankItems.Add(wxString::FromUTF8("グレートソード"));
    rankItems.Add(wxString::FromUTF8("フランベルジュ"));
    rankItems.Add(wxString::FromUTF8("センチネル"));
    rankItems.Add(wxString::FromUTF8("ハイペリオン"));
    rankItems.Add(wxString::FromUTF8("始まりの剣"));
    rankItems.Add(wxString::FromUTF8("始まりの剣⭐︎"));
        rankChoices = new wxChoice(
            basicInfoWindow,
            wxID_ANY,
            wxDefaultPosition,
            wxSize(150, -1),
            rankItems);
    rankChoices->SetSelection(0);
    faithSizer->Add(rankChoices, 0, wxALIGN_CENTER);

    basicInfoSizer->Add(nameSizer, 0, wxEXPAND | wxALL, 3);
    basicInfoSizer->Add(speciesSizer, 0, wxEXPAND | wxALL, 3);
    basicInfoSizer->Add(spfeatureSizer, 0, wxEXPAND | wxALL, 3);
    basicInfoSizer->Add(faithSizer, 0, wxEXPAND | wxALL, 3);

    // 折り返しの一時まとめ
    sizer->Add(basicInfoPane, 0, wxEXPAND | wxALL, 3);

    // -------------------------------------------------

    //能力値~技能までの折りたたみ
    wxCollapsiblePane* statusPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---能力値---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );
    
    collapsiblePanes.push_back(statusPane);

    // 折りたたみ内部
    this->statusWindow = statusPane->GetPane();

    wxBoxSizer* statusSizer = new wxBoxSizer(wxVERTICAL);

    this->statusWindow->SetSizer(statusSizer);

    statusPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    statusPane->Expand();
    
    // wxStaticText* abi = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---能力値---"));
    // sizer->Add(abi, 0, wxALL, 10);
    // wxStaticText* comment = new wxStaticText(statusWindow, wxID_ANY, wxString::FromUTF8("命中力などは技能、武具の中で最大値を参照"));
    // comment->SetForegroundColour(*wxRED); // 赤色に設定
    // statusSizer->Add(comment, 0, wxALIGN_RIGHT | wxBOTTOM, 5);

    // 親Sizerを横並びに設定
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main2Sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* main1Sizer = new wxBoxSizer(wxVERTICAL);
    // bornTextBoxes（技・体・心）の初期化
    wxBoxSizer* bornSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bornLabels[3] = { wxString::FromUTF8("(技"), wxString::FromUTF8("(体"), wxString::FromUTF8("(心") };
    for (int i = 0; i < 3; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, bornLabels[i] + wxString::FromUTF8(" "));
        bornSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        bornTextBoxes[i] = new wxTextCtrl(statusWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, 68));
        bornSizer->Add(bornTextBoxes[i], 0, wxALIGN_LEFT | wxBOTTOM, 7);  // 中央揃えではなく左揃え

        // テキストを中央揃えに設定
        bornTextBoxes[i]->SetWindowStyleFlag(wxTE_CENTER);
        bornTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    // abilityTextBoxes（A～F）の初期化
    wxBoxSizer* abilitySizer = new wxBoxSizer(wxVERTICAL);
    const wxString abilityLabels[6] = { wxString::FromUTF8("+A"), wxString::FromUTF8("+B"), wxString::FromUTF8("+C"), wxString::FromUTF8("+D"), wxString::FromUTF8("+E"), wxString::FromUTF8("+F") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, abilityLabels[i] + wxString::FromUTF8(" "));
        abilitySizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        abilityTextBoxes[i] = new wxTextCtrl(statusWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        abilitySizer->Add(abilityTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        abilityTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::UpdateAttribute, this);
    }

    //成長値の項目
    wxBoxSizer* growthSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, wxString::FromUTF8("+成長)"));
        growthSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // スピンボタンを追加
        growthSpinCtrl[i] = new wxSpinCtrl(statusWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        growthSizer->Add(growthSpinCtrl[i], 0, wxALIGN_LEFT| wxBOTTOM, 5);  // 中央揃えではなく左揃え
        growthSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::abilityChange, this);
    }

    // attributeTextBoxes（器用～精神）の初期化
    wxBoxSizer* attributeSizer = new wxBoxSizer(wxVERTICAL);
    const wxString attributeLabels[6] = { wxString::FromUTF8("=(器用度"), wxString::FromUTF8("=(敏捷度"), wxString::FromUTF8("=(筋力"), wxString::FromUTF8("=(生命力"), wxString::FromUTF8("=(知力"), wxString::FromUTF8("=(精神力") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, attributeLabels[i] + wxString::FromUTF8(" "));
        attributeSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        attributeTextBoxes[i] = new wxTextCtrl(statusWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
        attributeSizer->Add(attributeTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        attributeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::abilityChange, this);
    }
    
    //増強値の項目
    wxBoxSizer* increaseSizer = new wxBoxSizer(wxVERTICAL);
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, wxString::FromUTF8("+増強)"));
        increaseSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // スピンボタンを追加
        increaseSpinCtrl[i] = new wxSpinCtrl(statusWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 99, 0);
        increaseSizer->Add(increaseSpinCtrl[i], 0, wxALIGN_LEFT| wxBOTTOM, 5);  // 中央揃えではなく左揃え
        increaseSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::Updatebonus, this);
    }

    // bonusTextBoxes（器用～精神ボーナス）の初期化
    wxBoxSizer* bonusSizer = new wxBoxSizer(wxVERTICAL);
    const wxString bonusLabels[6] = { wxString::FromUTF8("/6→器用度B"), wxString::FromUTF8("/6→敏捷度B"), wxString::FromUTF8("/6→筋力B"), wxString::FromUTF8("/6→生命力B"), wxString::FromUTF8("/6→知力B"), wxString::FromUTF8("/6→精神力B") };
    for (int i = 0; i < 6; ++i) {
        // ラベルを追加
        wxStaticText* label = new wxStaticText(statusWindow, wxID_ANY, bonusLabels[i] + wxString::FromUTF8(" "));
        bonusSizer->Add(label, 0, wxALIGN_LEFT | wxBOTTOM, 5);  // ラベルの下にスペースを追加

        // テキストボックスを追加
        bonusTextBoxes[i] = new wxTextCtrl(statusWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(75, -1));
        bonusSizer->Add(bonusTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        bonusTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }


    
   // HP, MP, 抵抗力
    wxBoxSizer* hpSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* hpGridSizer = new wxFlexGridSizer(
        2,  // 2列
        0,  // 行数は自動
        10, // 縦間隔
        20  // 横間隔
    );

    const wxString hpLabels[4] = {
        wxString::FromUTF8("HP"),
        wxString::FromUTF8("MP"),
        wxString::FromUTF8("生命抵抗力"),
        wxString::FromUTF8("精神抵抗力")
    };

    for (int i = 0; i < 4; ++i) {

        // 1項目分の縦Sizer
        wxBoxSizer* valueSizer = new wxBoxSizer(wxVERTICAL);

        // ラベル
        wxStaticText* label = new wxStaticText(
            statusWindow,
            wxID_ANY,
            hpLabels[i]
        );

        valueSizer->Add(
            label,
            0,
            wxALIGN_LEFT | wxBOTTOM,
            2
        );

        // テキストボックス
        hpTextBoxes[i] = new wxTextCtrl(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("0"),
            wxDefaultPosition,
            wxSize(60, -1)
        );

        valueSizer->Add(
            hpTextBoxes[i],
            0,
            wxALIGN_LEFT
        );

        // Gridの1セルとして追加
        hpGridSizer->Add(
            valueSizer,
            0,
            wxALIGN_LEFT
        );
    }
    hpSizer->Add(
        hpGridSizer,
        0,
        wxALIGN_LEFT
    );

    // 通常移動
    wxFlexGridSizer* moveGridSizer = new wxFlexGridSizer(
    2,  // 2列
    0,  // 行数は自動
    10, // 縦方向の間隔
    20  // 横方向の間隔
    );

    wxBoxSizer* moveValueSizer = new wxBoxSizer(wxVERTICAL);

    moveValueSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("移動力")
        ),
        0,
        wxBOTTOM,
        2
    );

    wxBoxSizer* moveInputSizer = new wxBoxSizer(wxHORIZONTAL);

    moveTextBox = new wxTextCtrl(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("0"),
        wxDefaultPosition,
        wxSize(50, -1)
    );

    moveInputSizer->Add(
        moveTextBox,
        0,
        wxALIGN_CENTER_VERTICAL
    );

    moveInputSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("m")
        ),
        0,
        wxALIGN_CENTER_VERTICAL | wxLEFT,
        3
    );

    moveValueSizer->Add(
        moveInputSizer,
        0
    );

    moveGridSizer->Add(
        moveValueSizer,
        0,
        wxALIGN_LEFT
    );


    // 全力移動
    wxBoxSizer* fullmoveValueSizer = new wxBoxSizer(wxVERTICAL);
    fullmoveValueSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("*3 = 全力移動")
        ),
        0,
        wxBOTTOM,
        2
    );

    wxBoxSizer* fullmoveInputSizer = new wxBoxSizer(wxHORIZONTAL);

    fullmoveTextBox = new wxTextCtrl(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("0"),
        wxDefaultPosition,
        wxSize(50, -1)
    );

    fullmoveInputSizer->Add(
        fullmoveTextBox,
        0,
        wxALIGN_CENTER_VERTICAL
    );

    fullmoveInputSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("m")
        ),
        0,
        wxALIGN_CENTER_VERTICAL | wxLEFT,
        3
    );

    fullmoveValueSizer->Add(
        fullmoveInputSizer,
        0
    );

    moveGridSizer->Add(
        fullmoveValueSizer,
        0,
        wxALIGN_LEFT
    );


    // 制限移動
    wxBoxSizer* restmoveValueSizer = new wxBoxSizer(wxVERTICAL);
    restmoveValueSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("制限移動")
        ),
        0,
        wxBOTTOM,
        2
    );

    wxBoxSizer* restmoveInputSizer = new wxBoxSizer(wxHORIZONTAL);

    restmoveTextBox = new wxTextCtrl(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("3"),
        wxDefaultPosition,
        wxSize(50, -1)
    );

    restmoveInputSizer->Add(
        restmoveTextBox,
        0,
        wxALIGN_CENTER_VERTICAL
    );

    restmoveInputSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("m")
        ),
        0,
        wxALIGN_CENTER_VERTICAL | wxLEFT,
        3
    );

    restmoveValueSizer->Add(
        restmoveInputSizer,
        0
    );

    moveGridSizer->Add(
        restmoveValueSizer,
        0,
        wxALIGN_LEFT
    );

    hpSizer->Add(
        moveGridSizer,
        0,
        wxALIGN_LEFT | wxTOP,
        10
    );

    wxBoxSizer* armorValueSizer = new wxBoxSizer(wxVERTICAL);
    armorValueSizer->Add(
        new wxStaticText(
            statusWindow,
            wxID_ANY,
            wxString::FromUTF8("防護点")
        ),
        0,
        wxBOTTOM,
        2
    );

    armorSpinCtrl = new wxSpinCtrl(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8(""),
        wxDefaultPosition,
        wxSize(50, -1)
    );

    armorValueSizer->Add(
        armorSpinCtrl,
        0,
        wxALIGN_LEFT | wxTOP,
        10
    );

    hpSizer->Add(
        armorValueSizer,
        0,
        wxALIGN_LEFT | wxTOP,
        10
    );

    // 横並びにする
    main2Sizer->Add(bornSizer, 0, wxALIGN_CENTER);   
    main2Sizer->Add(abilitySizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(growthSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(attributeSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(increaseSizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    main2Sizer->Add(bonusSizer, 0, wxALIGN_CENTER | wxLEFT, 15);

    main1Sizer->Add(main2Sizer, 0, wxALIGN_CENTER| wxBOTTOM, 25);
    
    mainSizer->Add(main1Sizer, 0, wxALIGN_TOP);
    mainSizer->Add(hpSizer, 0, wxALIGN_TOP | wxLEFT, 30);
    // 判定パッケージなどの追加
    packageDetailSizer = new wxBoxSizer(wxVERTICAL);
    // mainSizerを折り返しSizerに追加
    statusSizer->Add(mainSizer, 0, wxEXPAND | wxALL, 2);
    statusSizer->Add(packageDetailSizer, 0, wxALIGN_TOP | wxALL, 5);
    // 折り返しの一時まとめ
    sizer->Add(statusPane, 0, wxEXPAND | wxALL, 3);

   // -------------------------------------------------

    //技能~言語までの折りたたみ
    wxCollapsiblePane* skillPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---技能・特技---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(skillPane);

    // 折りたたみ内部
    skillWindow = skillPane->GetPane();

    wxBoxSizer* skillSizer = new wxBoxSizer(wxVERTICAL);

    skillWindow->SetSizer(skillSizer);

    skillPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    skillPane->Expand();

    // wxStaticText* tec = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---技能・特技---"));
    // sizer->Add(tec, 0, wxALL, 10);


    // 親Sizerを横並びに設定
    wxBoxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* ssubSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sub1Sizer = new wxBoxSizer(wxHORIZONTAL);

    // 技能の初期化
    wxBoxSizer* techSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* techlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("技能 "));
    techSizer->Add(techlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        techTextBoxes[i] = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        techSizer->Add(techTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        techTextBoxes[i]->Bind(
            wxEVT_TEXT,
            [this](wxCommandEvent& event)
            {
                // RebuildSkillDetailUI();
                Updatehp(event);
                event.Skip();
            }
        );
     }

    // 技能テーブルの初期化
    wxBoxSizer* tableSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* tablelabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("テーブル "));
    tableSizer->Add(tablelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // ラジオボタンを追加
        wxBoxSizer* subtableSizer = new wxBoxSizer(wxHORIZONTAL);
        tableradioA[i] = new wxRadioButton(skillWindow, wxID_ANY, wxString::FromUTF8("A"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        tableradioB[i] = new wxRadioButton(skillWindow, wxID_ANY, wxString::FromUTF8("B"));
        
        subtableSizer->Add(tableradioA[i], 0, wxALIGN_CENTER| wxTOP | wxBOTTOM, 1);
        subtableSizer->Add(tableradioB[i], 0, wxALIGN_CENTER| wxTOP | wxBOTTOM, 1);
        tableSizer->Add(subtableSizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 4);
        // イベントを登録
        tableradioA[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
        tableradioB[i]->Bind(wxEVT_RADIOBUTTON, &Tab1Panel::Updateexp, this);
    }
    

    // 技能レベルの初期化
    wxBoxSizer* techlevelSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* techlevellabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("レベル "));
    techlevelSizer->Add(techlevellabel, 0, wxALIGN_CENTER | wxBOTTOM, 3);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // スピンボタンを追加
        techlevelSpinCtrl[i] = new wxSpinCtrl(skillWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(40, -1), wxSP_ARROW_KEYS, 0, 15, 0);
        techlevelSizer->Add(techlevelSpinCtrl[i], 0, wxALIGN_LEFT| wxTOP | wxBOTTOM, 2);  // 中央揃えではなく左揃え
        techlevelSpinCtrl[i]->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnSpinCtrlChange, this);
     }

    // 必要経験値の初期化
    wxBoxSizer* expSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* explabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("使用経験値 "));
    expSizer->Add(explabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        expTextBoxes[i] = new wxTextCtrl(skillWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
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
       
    allexpSizer->Add(new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("取得経験値: ")), 0, wxALIGN_CENTER_VERTICAL);
    getexpSpinCtrl = new wxSpinCtrl(skillWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100000000, 0);
    getexpSpinCtrl->SetIncrement(50); // 変化幅を50に設定
    getexpSpinCtrl->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    getexpSpinCtrl->Bind(wxEVT_SPINCTRL, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(getexpSpinCtrl, 0, wxALIGN_CENTER | wxRIGHT, 6);
    
    allexpSizer->Add(new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("合計使用経験値: ")), 0, wxALIGN_CENTER_VERTICAL );
    allexpTextBox = new wxTextCtrl(skillWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
    allexpTextBox->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    allexpSizer->Add(allexpTextBox, 0, wxALIGN_CENTER);

    // 冒険者レベル、取得経験値、合計使用経験値
    wxBoxSizer* levelSizer = new wxBoxSizer(wxHORIZONTAL);
    levelSizer->Add(new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("冒険者レベル: ")), 0, wxALIGN_CENTER_VERTICAL );
    levelTextBox = new wxTextCtrl(skillWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
    levelTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    levelSizer->Add(levelTextBox, 0, wxALIGN_CENTER);

    warningText = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("取得経験値を超過しています！"), wxPoint(10, 10));
    warningText->Bind(wxEVT_TEXT, &Tab1Panel::OnCompareValues, this);
    warningText->SetForegroundColour(*wxRED); // 赤色に設定
    warningText->Hide(); // 初期状態では非表示にする
    levelSizer->Add(warningText, 0, wxALIGN_CENTER| wxLEFT, 10);

    ssubSizer->Add(sub1Sizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(allexpSizer, 0, wxALIGN_CENTER);
    ssubSizer->Add(levelSizer, 0, wxALIGN_LEFT);

    wxBoxSizer* sub2Sizer = new wxBoxSizer(wxHORIZONTAL);

    talentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText * talentlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("戦闘特技"));
    talentSizer->Add(talentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 8; ++i) {
        wxString talentText = wxString::Format(wxString::FromUTF8("%d: "), 2 * i + 1); // 数値を文字列に変換
        // テキストボックスを追加
        wxTextCtrl* talentBox =new wxTextCtrl(
            skillWindow,
            wxID_ANY,
            talentText,
            wxDefaultPosition,
            wxSize(120, -1)
        );
        talentTextBoxes.push_back(talentBox);
        talentSizer->Add(talentBox, 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        talentBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    abstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* abstlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("効果概要"));
    abstSizer->Add(abstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    for (int i = 0; i < 8; ++i) {
        // テキストボックスを追加
         wxTextCtrl* abstBox = new wxTextCtrl(
            skillWindow,
            wxID_ANY,
            "",
            wxDefaultPosition,
            wxSize(120, -1)
        );
        abstTextBoxes.push_back(abstBox);
        abstSizer->Add(abstBox, 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }
    extraTalentSizer = new wxBoxSizer(wxVERTICAL);
    extraAbstSizer = new wxBoxSizer(wxVERTICAL);

    talentSizer->Add(
        extraTalentSizer,
        0,
        wxALIGN_LEFT
    );

    abstSizer->Add(
        extraAbstSizer,
        0,
        wxALIGN_LEFT
    );

    // 横並びにする
    sub2Sizer->Add(talentSizer, 0, wxALIGN_CENTER);
    sub2Sizer->Add(abstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);

    //sub1Sizerとsub2SeizerをsubSeizerにまとめる
    subSizer->Add(ssubSizer, 0, wxALIGN_TOP);
    subSizer->Add(sub2Sizer, 0, wxALIGN_TOP | wxLEFT, 20);

    // // subSizerを親Sizerに追加
    skillSizer->Add(subSizer, 0, wxEXPAND | wxLEFT, 3);

    // 練技などの追加習得欄
    skillDetailSizer = new wxBoxSizer(wxHORIZONTAL);
    skillSizer->Add(skillDetailSizer, 0, wxALIGN_TOP | wxALL, 5);

    // 記述欄の設定
    freespaceSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* freeSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* freetalentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* freetalentlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("自動取得など"));
    freetalentSizer->Add(freetalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    freetalentTextBox = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freetalentTextBox->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    freetalentSizer->Add(freetalentTextBox, 0, wxALIGN_LEFT | wxLEFT, 3);

    wxBoxSizer* freeabstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* freeabstlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("効果概要"));
    freeabstSizer->Add(freeabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    freeabstTextBox = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    freeabstSizer->Add(freeabstTextBox, 0, wxALIGN_LEFT);

    freeSizer->Add(freetalentSizer, 0, wxALIGN_CENTER);
    freeSizer->Add(freeabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);

    wxBoxSizer* magicSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* magictalentSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* magictalentlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("よく使う魔法など"));
    magictalentSizer->Add(magictalentlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    magictalentTextBox = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magictalentSizer->Add(magictalentTextBox, 0, wxALIGN_LEFT);

    wxBoxSizer* magicabstSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* magicabstlabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("MP, 効果概要"));
    magicabstSizer->Add(magicabstlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    magicabstTextBox = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(110, 150), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    magicabstSizer->Add(magicabstTextBox, 0, wxALIGN_LEFT);

    magicSizer->Add(magictalentSizer, 0, wxALIGN_CENTER);
    magicSizer->Add(magicabstSizer, 0, wxALIGN_CENTER | wxLEFT, 0);

    wxBoxSizer* battleSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* battlelabel = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("戦闘関連メモ"));
    wxStaticText* battlelabel2 = new wxStaticText(skillWindow, wxID_ANY, wxString::FromUTF8("(受けた効果など)"));
    battleSizer->Add(battlelabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    battleSizer->Add(battlelabel2, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    battleTextBox = new wxTextCtrl(skillWindow, wxID_ANY, "", wxDefaultPosition, wxSize(110, 131), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    battleSizer->Add(battleTextBox, 0, wxALIGN_LEFT);
        
    freespaceSizer->Add(freeSizer, 0, wxALIGN_CENTER);
    freespaceSizer->Add(magicSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    freespaceSizer->Add(battleSizer, 0, wxALIGN_CENTER | wxLEFT, 5);
    
    skillSizer->Add(freespaceSizer, 0, wxALIGN_LEFT | wxTOP, 15);

    placeholderSizer = new wxBoxSizer(wxVERTICAL); // その他技能の入る場所

    otherskillSizer = new wxBoxSizer(wxHORIZONTAL);
    placeholderSizer->Add(otherskillSizer, 0, wxALIGN_LEFT);
    skillSizer->Add(placeholderSizer, 0, wxALL, 5);

    // 折り返しの一時まとめ
    sizer->Add(skillPane, 0, wxALL, 5);

    // -------------------------------------------------

    //言語~武具までの折りたたみ
    wxCollapsiblePane* commandPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---言語・経歴---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(commandPane);

        // 折りたたみ内部
    wxWindow* commandWindow = commandPane->GetPane();

    wxBoxSizer* commandSizer = new wxBoxSizer(wxVERTICAL);

    commandWindow->SetSizer(commandSizer);

    commandPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    commandPane->Expand();

    // 親Sizerを横並びに設定
    trioSizer = new wxBoxSizer(wxHORIZONTAL);

    // 言語の初期化
    wxBoxSizer* langSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* langlabel = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("言語"));
    langSizer->Add(langlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString langtext[10] = { wxString::FromUTF8("交易共通語"), wxString::FromUTF8("汎用蛮族語"), wxString::FromUTF8("地方語(ブルライト)"), wxString::FromUTF8("種族言語( )"), wxString::FromUTF8("魔法文明語"), wxString::FromUTF8("魔動機文明語"), wxString::FromUTF8("妖精語"), wxString::FromUTF8("神紀文明語"), wxString::FromUTF8("魔神語"), ""};
    for (int i = 0; i < 10; ++i) {
        // テキストボックスを追加
        langTextBoxes[i] = new wxTextCtrl(commandWindow, wxID_ANY, langtext[i], wxDefaultPosition, wxSize(110, -1));
        langSizer->Add(langTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    // 会話の初期化
    wxBoxSizer* speakSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* speaklabel = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("会話"));
    speakSizer->Add(speaklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 10; ++i) {
        // チェックボックスを追加
        speakcheckBoxes[i] = new wxCheckBox(commandWindow, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        speakSizer->Add(speakcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 5);
    }
    speakcheckBoxes[0]->SetValue(true);

    // 読文の初期化
    wxBoxSizer* readSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* readlabel = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("読文"));
    readSizer->Add(readlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 10; ++i) {
        // チェックボックスを追加
        readcheckBoxes[i] = new wxCheckBox(commandWindow, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1));
        readSizer->Add(readcheckBoxes[i], 0, wxCENTER | wxTOP | wxBOTTOM, 5);
    }
    readcheckBoxes[0]->SetValue(true);

    //sub1Sizerとsub2SeizerをsubSeizerにまとめる
    trioSizer->Add(langSizer, 0, wxALIGN_TOP | wxLEFT, 2);
    trioSizer->Add(speakSizer, 0, wxALIGN_TOP | wxLEFT, 1);
    trioSizer->Add(readSizer, 0, wxALIGN_TOP | wxLEFT, 1);

    wxBoxSizer* otherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* otherlabel = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("経歴、その他メモなど"));
    otherSizer->Add(otherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 2);  // ラベルの下にスペースを追加
    otherTextBox = new wxTextCtrl(commandWindow, wxID_ANY, "", wxDefaultPosition, wxSize(380, 100), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    otherSizer->Add(otherTextBox, 0, wxALIGN_LEFT);

    // ラベルを追加
    wxStaticText* chatlabel = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("個人チャットパレット"));
    otherSizer->Add(chatlabel, 0, wxALIGN_CENTER | wxBOTTOM, 0); 
    // 注意書き
    wxStaticText* chatcomment = new wxStaticText(commandWindow, wxID_ANY, wxString::FromUTF8("ここに記入したテキストはチャットパレット先頭に自動入力されます"));
    chatcomment->SetForegroundColour(*wxRED); // 赤色に設定
    otherSizer->Add(chatcomment, 0, wxALIGN_LEFT | wxBOTTOM, 1);
    chatTextBox = new wxTextCtrl(commandWindow, wxID_ANY, "", wxDefaultPosition, wxSize(380, 135), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    otherSizer->Add(chatTextBox, 0, wxALIGN_LEFT);
    
    // otherSizerをtrioSizerに追加
    trioSizer->Add(otherSizer, 0, wxALIGN_TOP | wxLEFT, 5);

    // subSizerを親Sizerに追加
    commandSizer->Add(trioSizer, 0, wxEXPAND | wxTOP, 15);

    // 折り返しの一時まとめ
    sizer->Add(commandPane, 0, wxALL, 5);

    // -------------------------------------------------

    //マテカ・指輪・ポーションなどまでの折りたたみ
    wxCollapsiblePane* consumptionPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---消費アイテム---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(consumptionPane);
    
        // 折りたたみ内部
    wxWindow* consumptionWindow = consumptionPane->GetPane();

    wxBoxSizer* consumptionSizer = new wxBoxSizer(wxVERTICAL);

    consumptionWindow->SetSizer(consumptionSizer);

    consumptionPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    consumptionPane->Expand();

    // 横並びのため
    wxBoxSizer* consump2Sizer = new wxBoxSizer(wxHORIZONTAL);
    // マテカの列 
    wxBoxSizer* matecaSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* mateca = new wxStaticText(consumptionWindow, wxID_ANY, wxString::FromUTF8("マテリアルカード"));
    matecaSizer->Add(mateca, 0, wxALL, 0);

    const wxString rowLabels[5] = {
         wxString::FromUTF8("赤"),  wxString::FromUTF8("緑"),  wxString::FromUTF8("黒"),  wxString::FromUTF8("白"),  wxString::FromUTF8("金")
    };

    const wxString colLabels[4] = {
         wxString::FromUTF8("B"),  wxString::FromUTF8("A"),  wxString::FromUTF8("S"),  wxString::FromUTF8("SS")
    };

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(
        6,   // 列ラベルの行を含む
        5,   // 行ラベルの列を含む
        5,
        5
    );


    gridSizer->Add(
        new wxStaticText(
            consumptionWindow,
            wxID_ANY,
            ""
        ),
        0,
        wxEXPAND
    );

    for (int col = 0; col < 4; ++col) {

        wxStaticText* colLabel = new wxStaticText(
            consumptionWindow,
            wxID_ANY,
            colLabels[col],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        wxColour colColours[4] = {
            wxColour(139, 69, 19),   // B
            *wxBLUE,                  // A
            *wxGREEN,                 // S
            wxColour(128, 0, 128)    // SS
        };

        colLabel->SetForegroundColour(colColours[col]);

        gridSizer->Add(
            colLabel,
            0,
            wxALIGN_CENTER | wxALL,
            5
        );
    }

    for (int row = 0; row < 5; ++row) {

        // 行ラベル
        wxStaticText* rowLabel = new wxStaticText(
            consumptionWindow,
            wxID_ANY,
            rowLabels[row],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        wxColour rowColours[5] = {
            *wxRED,
            *wxGREEN,
            *wxBLACK,
            wxColour(160, 160, 160), // 灰色
            wxColour(218, 165, 32)   // 金色
        };

        rowLabel->SetForegroundColour(rowColours[row]);

        gridSizer->Add(
            rowLabel,
            0,
            wxEXPAND | wxALIGN_CENTER | wxALL,
            5
        );

        for (int col = 0; col < 4; ++col) {

            matecaSpinCtrl[row][col] = new wxSpinCtrl(
                consumptionWindow,
                wxID_ANY,
                wxEmptyString,
                wxDefaultPosition,
                wxSize(50, -1),
                wxSP_ARROW_KEYS,
                -99,
                99,
                0
            );

            gridSizer->Add(
                matecaSpinCtrl[row][col],
                0,
                wxEXPAND | wxALL,
                2
            );
        }
    }

    matecaSizer->Add(gridSizer, 0, wxALIGN_CENTER);

    // ポーションの列 
    wxBoxSizer* potionSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* potion = new wxStaticText(consumptionWindow, wxID_ANY, wxString::FromUTF8("薬草・ポーション"));
    potionSizer->Add(potion, 0, wxALL, 0);

    const wxString col2Labels[2] = {
         wxString::FromUTF8("名称"),  wxString::FromUTF8("個数")
    };

    wxFlexGridSizer* grid2Sizer = new wxFlexGridSizer(
        7,   // 列ラベルの行を含む
        2, 5, 5
    );

    for (int col = 0; col < 2; ++col) {

        wxStaticText* col2Label = new wxStaticText(
            consumptionWindow,
            wxID_ANY,
            col2Labels[col],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        grid2Sizer->Add(
            col2Label,
            0,
            wxALIGN_CENTER | wxALL,
            2
        );
    }

    for (int row = 0; row < 6; ++row) {
        
        potionTextBoxes[row] = new wxTextCtrl(
            consumptionWindow,
            wxID_ANY,
            "", 
            wxDefaultPosition, 
            wxSize(100, -1)
        );

        grid2Sizer->Add(
            potionTextBoxes[row],
            0,
            wxEXPAND | wxALL,
            0
        );

        potionSpinCtrl[row] = new wxSpinCtrl(
            consumptionWindow,
            wxID_ANY,
            wxEmptyString,
            wxDefaultPosition,
            wxSize(40, -1),
            wxSP_ARROW_KEYS,
            -99,
            99,
            0
        );

        grid2Sizer->Add(
            potionSpinCtrl[row],
            0,
            wxEXPAND | wxALL,
            0
        );
    }

    potionSizer->Add(grid2Sizer, 0, wxALIGN_CENTER);

    // 指輪/腕輪とその他の列 
    wxBoxSizer* ringSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* ring = new wxStaticText(consumptionWindow, wxID_ANY, wxString::FromUTF8("指輪/腕輪・その他"));
    ringSizer->Add(ring, 0, wxALL, 0);

    wxFlexGridSizer* grid3Sizer = new wxFlexGridSizer(
        7,   // 列ラベルの行を含む
        2, 5, 5
    );

    for (int col = 0; col < 2; ++col) {

        wxStaticText* col3Label = new wxStaticText(
            consumptionWindow,
            wxID_ANY,
            col2Labels[col],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        grid3Sizer->Add(
            col3Label,
            0,
            wxALIGN_CENTER | wxALL,
            2
        );
    }

    for (int row = 0; row < 6; ++row) {
        
        ringTextBoxes[row] = new wxTextCtrl(
            consumptionWindow,
            wxID_ANY,
            "", 
            wxDefaultPosition, 
            wxSize(100, -1)
        );

        grid3Sizer->Add(
            ringTextBoxes[row],
            0,
            wxEXPAND | wxALL,
            0
        );

        ringSpinCtrl[row] = new wxSpinCtrl(
            consumptionWindow,
            wxID_ANY,
            wxEmptyString,
            wxDefaultPosition,
            wxSize(40, -1),
            wxSP_ARROW_KEYS,
            -99,
            99,
            0
        );

        grid3Sizer->Add(
            ringSpinCtrl[row],
            0,
            wxEXPAND | wxALL,
            0
        );
    }

    ringSizer->Add(grid3Sizer, 0, wxALIGN_CENTER);


    // 横並びにする
    consump2Sizer->Add(matecaSizer, 0, wxALIGN_CENTER);
    consump2Sizer->Add(potionSizer, 0, wxALIGN_CENTER);
    consump2Sizer->Add(ringSizer, 0, wxALIGN_CENTER);

    // 折りたたみに追加
    consumptionSizer->Add(consump2Sizer, 0, wxEXPAND | wxALL, 0);

    // 折り返しの一時まとめ
    sizer->Add(consumptionPane, 0, wxALL, 5);


    // -------------------------------------------------

    //武具~最後までの折りたたみ
    wxCollapsiblePane* goodsPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---武具・所持品---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(goodsPane);
    
        // 折りたたみ内部
    wxWindow* goodsWindow = goodsPane->GetPane();

    wxBoxSizer* goodsSizer = new wxBoxSizer(wxVERTICAL);

    goodsWindow->SetSizer(goodsSizer);

    goodsPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    goodsPane->Expand();


    // wxStaticText* item = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("---武具・所持品---"));
    // sizer->Add(item, 0, wxALL, 10);

    // 所持品欄の初期化
    wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

    // 武器欄の初期化
    wxBoxSizer* weaponSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* weaponnameSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponnamelabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("武器"));
    weaponnameSizer->Add(weaponnamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        weaponnameTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(90, 50));
        weaponnameSizer->Add(weaponnameTextBoxes[i], 0, wxALIGN_LEFT| wxTOP | wxBOTTOM, 1);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponuseSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponuselabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("用法"));
    weaponuseSizer->Add(weaponuselabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponuseTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weaponuseSizer->Add(weaponuseTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponstrSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponstrlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("必筋"));
    weaponstrSizer->Add(weaponstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponstrTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(40, -1));
        weaponstrSizer->Add(weaponstrTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* weaponhitSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponhitlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("命中力"));
    weaponhitSizer->Add(weaponhitlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponhitTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        weaponhitSizer->Add(weaponhitTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        weaponhitTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* weapondmSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weapondmlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("威力"));
    weapondmSizer->Add(weapondmlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weapondmTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        weapondmSizer->Add(weapondmTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }


    wxBoxSizer* weaponcritSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponcritlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("C値"));
    weaponcritSizer->Add(weaponcritlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponcritTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("11"), wxDefaultPosition, wxSize(50, -1));
        weaponcritSizer->Add(weaponcritTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    // 武器種選択
    wxBoxSizer* weapontypeSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* weapontypeLabel =
        new wxStaticText(
            goodsWindow,
            wxID_ANY,
            wxString::FromUTF8("武器種")
        );
    weapontypeSizer->Add(
        weapontypeLabel,
        0,
        wxALIGN_CENTER | wxBOTTOM, 5
    );
    wxArrayString weapontypeItems;
    weapontypeItems.Add(wxString::FromUTF8("--"));
    weapontypeItems.Add(wxString::FromUTF8("ソード"));
    weapontypeItems.Add(wxString::FromUTF8("アックス"));
    weapontypeItems.Add(wxString::FromUTF8("スピア"));
    weapontypeItems.Add(wxString::FromUTF8("メイス"));
    weapontypeItems.Add(wxString::FromUTF8("スタッフ"));
    weapontypeItems.Add(wxString::FromUTF8("フレイル"));
    weapontypeItems.Add(wxString::FromUTF8("ウォーハンマー"));
    weapontypeItems.Add(wxString::FromUTF8("格闘"));
    weapontypeItems.Add(wxString::FromUTF8("投擲"));
    weapontypeItems.Add(wxString::FromUTF8("ボウ"));
    weapontypeItems.Add(wxString::FromUTF8("クロスボウ"));
    weapontypeItems.Add(wxString::FromUTF8("ガン"));
    for (int i = 0; i < 3; ++i){
        weapontypeChoices[i] = new wxChoice(
            goodsWindow,
            wxID_ANY,
            wxDefaultPosition,
            wxSize(80, 50),
            weapontypeItems
        );

        weapontypeChoices[i]->SetSelection(0);
        weapontypeSizer->Add(
            weapontypeChoices[i],
            0,
            wxALIGN_LEFT
        );
    }
    
    wxBoxSizer* weaponaddSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponaddlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("追加D"));
    weaponaddSizer->Add(weaponaddlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 6; ++i) {
        // テキストボックスを追加
        weaponaddTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
        weaponaddSizer->Add(weaponaddTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        weaponaddTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* weaponotherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* weaponotherlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("備考"));
    weaponotherSizer->Add(weaponotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        weaponotherTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(130, 50), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
        weaponotherSizer->Add(weaponotherTextBoxes[i], 0, wxALIGN_LEFT| wxTOP | wxBOTTOM, 1);  // 中央揃えではなく左揃え
    }
    
    //weapon系列をまとめる
    weaponSizer->Add(weaponnameSizer, 0, wxALIGN_CENTER);
    weaponSizer->Add(weaponuseSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponhitSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weapondmSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponcritSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weapontypeSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponaddSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    weaponSizer->Add(weaponotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    // 防具欄と所持金欄をあわせる
    wxBoxSizer* armorSizer = new wxBoxSizer(wxHORIZONTAL);

    // 防具欄の初期化
    wxBoxSizer* subarmorSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* armornameSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armornamelabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("防具"));
    armornameSizer->Add(armornamelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString armortext[3] = { wxString::FromUTF8("鎧"), wxString::FromUTF8("盾"), wxString::FromUTF8("その他") };
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armornameTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, armortext[i] + wxString::FromUTF8(":"), wxDefaultPosition, wxSize(90, -1));
        armornameSizer->Add(armornameTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* armorstrSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorstrlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("必筋"));
    armorstrSizer->Add(armorstrlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorstrTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1));
        armorstrSizer->Add(armorstrTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    wxBoxSizer* armorevadeSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorevadelabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("回避修正"));
    armorevadeSizer->Add(armorevadelabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorevadeTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(50, -1));
        armorevadeSizer->Add(armorevadeTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        armorevadeTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    wxBoxSizer* armorblockSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorblocklabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("防護点"));
    armorblockSizer->Add(armorblocklabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorblockTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(40, -1));
        armorblockSizer->Add(armorblockTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
        armorblockTextBoxes[i]->Bind(wxEVT_TEXT, &Tab1Panel::Updatehp, this);
    }

    // 防具カテゴリ選択
    wxBoxSizer* armortypeSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* armortypeLabel =
        new wxStaticText(
            goodsWindow,
            wxID_ANY,
            wxString::FromUTF8("防具種")
        );
    armortypeSizer->Add(
        armortypeLabel,
        0,
        wxALIGN_CENTER | wxBOTTOM, 5
    );
    wxArrayString armortypeItems;
    armortypeItems.Add(wxString::FromUTF8("--"));
    armortypeItems.Add(wxString::FromUTF8("金属鎧"));
    armortypeItems.Add(wxString::FromUTF8("非金属鎧"));
    armortypeItems.Add(wxString::FromUTF8("盾"));
    armortypeItems.Add(wxString::FromUTF8("その他"));
    for (int i = 0; i < 3; ++i){
        armortypeChoices[i] = new wxChoice(
            goodsWindow,
            wxID_ANY,
            wxDefaultPosition,
            wxSize(80, -1),
            armortypeItems
        );

        armortypeChoices[i]->SetSelection(0);
        armortypeSizer->Add(
            armortypeChoices[i],
            0,
            wxALIGN_LEFT
        );
    }

    wxBoxSizer* armorotherSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* armorotherlabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("備考"));
    armorotherSizer->Add(armorotherlabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 3; ++i) {
        // テキストボックスを追加
        armorotherTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
        armorotherSizer->Add(armorotherTextBoxes[i], 0, wxALIGN_LEFT);  // 中央揃えではなく左揃え
    }

    //armor系列をまとめる
    subarmorSizer->Add(armornameSizer, 0, wxALIGN_CENTER);
    subarmorSizer->Add(armorstrSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorevadeSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorblockSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armortypeSizer, 0, wxALIGN_CENTER | wxLEFT, 0);
    subarmorSizer->Add(armorotherSizer, 0, wxALIGN_CENTER | wxLEFT, 0);


    // 金銭のテキストボックス
    wxBoxSizer* moneySizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* poketSizer = new wxBoxSizer(wxHORIZONTAL);
    poketSizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("所持金: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    poketSpinCtrl = new wxSpinCtrl(goodsWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 1410065408, 0);
    poketSizer->Add(poketSpinCtrl, 0, wxALIGN_CENTER);
    poketSizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("G")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* debtSizer = new wxBoxSizer(wxHORIZONTAL);
    debtSizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("預金/借金: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    debtTextBox = new wxTextCtrl(goodsWindow, wxID_ANY, wxString::FromUTF8("0"), wxDefaultPosition, wxSize(60, -1));
    debtSizer->Add(debtTextBox, 0, wxALIGN_CENTER);
    debtSizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("G")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    wxBoxSizer* glorySizer = new wxBoxSizer(wxHORIZONTAL);
    glorySizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("名誉点: ")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    glorySpinCtrl = new wxSpinCtrl(goodsWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 100000000, 0);
    glorySizer->Add(glorySpinCtrl, 0, wxALIGN_CENTER);
    glorySizer->Add(new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("点")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);

    moneySizer->Add(poketSizer, 0, wxALIGN_LEFT | wxBOTTOM, 2);
    moneySizer->Add(debtSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);
    moneySizer->Add(glorySizer, 0, wxALIGN_LEFT);

    armorSizer->Add(subarmorSizer, 0, wxALIGN_CENTER);
    armorSizer->Add(moneySizer, 0, wxALIGN_CENTER | wxLEFT, 15);
    

    itemSizer->Add(weaponSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);
    itemSizer->Add(armorSizer, 0, wxALIGN_LEFT | wxBOTTOM, 5);


    goodsSizer->Add(itemSizer, 0, wxEXPAND | wxLEFT, 2);

    wxBoxSizer* equipSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* acesSizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* acesSizer2 = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* aceslabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("各部位の装飾品"));
    acesSizer1->Add(aceslabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);  // ラベルの下にスペースを追加
    const wxString acesLabels[12] = { wxString::FromUTF8("頭"), wxString::FromUTF8("顔"), wxString::FromUTF8("耳"), wxString::FromUTF8("首"), wxString::FromUTF8("背中"), wxString::FromUTF8("右手"), wxString::FromUTF8("左手"), wxString::FromUTF8("腰"), wxString::FromUTF8("足"), wxString::FromUTF8("その他"), wxString::FromUTF8("その他2"), wxString::FromUTF8("その他3") };
    for (int i = 0; i < 12; ++i) { //装飾品名
        acesTextBoxes[i] = new wxTextCtrl(goodsWindow, wxID_ANY, acesLabels[i] + wxString::FromUTF8(": "), wxDefaultPosition, wxSize(150, -1));
        acesSizer1->Add(acesTextBoxes[i], 0, wxALIGN_LEFT);
    }
    acesTextBoxes[10]->Hide();
    acesTextBoxes[11]->Hide();
    
    // ラベルを追加
    wxStaticText* aceslabel2 = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("効果概要"));
    acesSizer2->Add(aceslabel2, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ラベルの下にスペースを追加
    for (int i = 0; i < 12; ++i) { // 装飾品の効果
        acesTextBoxes2[i] = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(175, -1));
        acesSizer2->Add(acesTextBoxes2[i], 0, wxALIGN_LEFT);
        
    }
    acesTextBoxes2[10]->Hide();
    acesTextBoxes2[11]->Hide();
    equipSizer->Add(acesSizer1, 0, wxALIGN_LEFT);
    equipSizer->Add(acesSizer2, 0, wxALIGN_LEFT);
    
    wxBoxSizer* bagSizer = new wxBoxSizer(wxVERTICAL);
    // ラベルを追加
    wxStaticText* baglabel = new wxStaticText(goodsWindow, wxID_ANY, wxString::FromUTF8("所持アイテムなど"));
    bagSizer->Add(baglabel, 0, wxALIGN_CENTER | wxBOTTOM , 5);  // ラベルの下にスペースを追加
    bagTextBox = new wxTextCtrl(goodsWindow, wxID_ANY, "", wxDefaultPosition, wxSize(250, 300), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    bagSizer->Add(bagTextBox, 0, wxALIGN_LEFT);

    equipSizer->Add(bagSizer, 0, wxLEFT, 5);

    goodsSizer->Add(equipSizer, 0, wxEXPAND);

    // 折り返しの一時まとめ
    sizer->Add(goodsPane, 0, wxALL, 5);

    SetSizer(sizer);
    Layout();
    FitInside();

    // 起動時のスクロール位置を先頭にする
    CallAfter([this]()
    {
        nameTextBox->SetFocus();
        Scroll(0, 0);
    });
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
    int lv = 0, aglity = 0, vital = 0, magic = 0, armor = 0, tarmor = 0, mplv = 0;
    long bonus[6]{}, magically[6]{};
    
    std::vector<wxString> classNameByIndex;
    std::fill(magically, magically + 6, 0);
    std::string armorType[3];
    bool enhancerAdded = false;  // 追加済みかどうかのフラグ

    // 前後の半角・全角スペースを除去する関数
    auto trimSpaces = [](wxString str) -> wxString {
        str.Trim(true);   // 末尾の空白を除去
        str.Trim(false);  // 先頭の空白を除去

        // 全角スペースを除去
        while (str.StartsWith(wxString::FromUTF8("　"))) {
            str = str.Mid(1);
        }
        while (str.EndsWith(wxString::FromUTF8("　"))) {
            str.RemoveLast();
        }
        return str;
    };

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


    for (int i = 0; i < 3; ++i) {
		armorblockTextBoxes[i]->GetValue().ToInt(&armor); // 防具の防護点を取得
        tarmor += armor;
        if (armortypeChoices[i]->GetSelection() != wxNOT_FOUND) {
            armorType[i] =
                armortypeChoices[i]->GetStringSelection().ToStdString();
        }
        else {
            armorType[i] = "";
        } // 防具種を取得
    }

    for (int i = 0; i < 6; ++i) {
        std::string className = techTextBoxes[i]->GetValue().ToStdString(); // 技能名を取得
        if (className.empty()) {
            //this->Layout();
            continue; // 技能名が空の場合はスキップ
        }

        else if (className == wxString::FromUTF8("ソーサラー") || className == wxString::FromUTF8("コンジャラー") || className == wxString::FromUTF8("プリースト") || className == wxString::FromUTF8("マギテック") || className == wxString::FromUTF8("フェアリーテイマー")) {
            magically[i] = techlevelSpinCtrl[i]->GetValue();
        }// 魔法使い系技能
 
        else {
            //this->Layout();
            continue; // 技能名が空の場合はスキップ
        }
    }
    // MPは合計する
    mplv = std::accumulate(magically, magically + 6, 0);

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

    // 特技欄の確認
    for (int i = 0; i < std::size(talentTextBoxes); ++i){
        std::string talentName =
            talentTextBoxes[i]->GetValue().ToStdString();

        if (talentName.empty())
            continue;
        
        else if (talentName.find(wxString::FromUTF8("足さばき")) != std::string::npos) {
            restmoveTextBox->SetValue(wxString::Format(wxString::FromUTF8("10"))); // 足さばきがある場合
        }
        else if (talentName.find(wxString::FromUTF8("頑強")) != std::string::npos) {
            vital += 15; // 頑強がある場合
        }
        else if (talentName.find(wxString::FromUTF8("超頑強")) != std::string::npos) {
            vital += 15; // 超頑強がある場合
        }
        else if (talentName.find(wxString::FromUTF8("タフネス")) != std::string::npos) {
            vital += 15; // タフネスがある場合
        }
        else if (talentName.find(wxString::FromUTF8("キャパシティ")) != std::string::npos) {
             magic+= 15; // キャパシティがある場合
        }
        else if (talentName.find(wxString::FromUTF8("防具習熟")) != std::string::npos) {
            wxString talent = trimSpaces(wxString::FromUTF8(talentName));
            int slashPos = talent.Find('/');
                if (slashPos != wxNOT_FOUND) {
                    wxString talentType = trimSpaces(talent.Mid(slashPos + 1));

                    if (talentType == armorType) {
                        if (talent.Contains(wxString::FromUTF8("防具習熟A"))) {
                            tarmor += 1;
                        }
                        else if (talent.Contains(wxString::FromUTF8("防具習熟S"))) {
                            tarmor += 2;
                        }
                    }
                }
        }
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

    armorSpinCtrl->SetValue(wxString::Format(wxString::FromUTF8("%ld"), (tarmor)));// 防護点

    moveTextBox->SetValue(wxString::Format(wxString::FromUTF8("%ld"), aglity)); // 移動力
    fullmoveTextBox->SetValue(wxString::Format(wxString::FromUTF8("%ld"), 3*aglity)); // 全力移動力
}
// 表追加の関数
void Tab1Panel::EnsureCombatGrid()
{
    if (combatGrid != nullptr) {
        return;
    }

    // 表のタイトル
    wxStaticText* title = new wxStaticText(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("戦闘技能")
    );

    packageDetailSizer->Add(
        title,
        0,
        wxALL,
        5
    );

    // 表を作成
    combatGrid = new wxGrid(
        statusWindow,
        wxID_ANY
    );

    // 0行、5列で作成
    combatGrid->CreateGrid(0, 5);

    combatGrid->SetColLabelValue(0, wxString::FromUTF8("技能名"));
    combatGrid->SetColLabelValue(1, wxString::FromUTF8("レベル"));
    combatGrid->SetColLabelValue(2, wxString::FromUTF8("命中力"));
    combatGrid->SetColLabelValue(3, wxString::FromUTF8("回避力"));
    combatGrid->SetColLabelValue(4, wxString::FromUTF8("追加D"));

    // セルの編集を禁止
    combatGrid->EnableEditing(false);

    packageDetailSizer->Add(
        combatGrid,
        0,
        wxEXPAND | wxALL,
        5
    );
}
// 行追加の関数
void Tab1Panel::AddCombatSkillRow(
    const wxString& className,
    int level,
    int hit,
    int evade,
    int damage,
    bool showEvade
)
{
    // 表が存在しなければ作成
    EnsureCombatGrid();
    // 行を1つ追加
    int row = combatGrid->GetNumberRows();
    combatGrid->AppendRows(1);
    combatGrid->SetRowLabelSize(0);
    // 各セルに値を設定
    combatGrid->SetCellValue(
        row, 0, className
    );
    combatGrid->SetCellValue(
        row, 1, wxString::Format("%d", level)
    );
    combatGrid->SetCellValue(
        row, 2, wxString::Format("%d", hit)
    );
    combatGrid->SetCellValue(
        row, 3,
        showEvade ? wxString::Format("%d", evade) : wxString("")
    );

    combatGrid->SetCellValue(
        row, 4, wxString::Format("%d", damage)
    );

    // セルを編集不可にする
    for (int col = 0; col < 5; ++col) {
        combatGrid->SetReadOnly(row, col);
    }

    // レイアウトを更新
    statusWindow->Layout();
    statusWindow->FitInside();
    // statusPane->Layout();
    Layout();
    FitInside();
}
// 魔法使い系の表追加
void Tab1Panel::EnsureMagicGrid()
{
    // すでに作成済みなら何もしない
    if (magicGrid != nullptr) {
        return;
    }

    // 表のタイトル
    wxStaticText* title = new wxStaticText(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("魔法使い系技能")
    );

    packageDetailSizer->Add(
        title,
        0,
        wxALL,
        5
    );

    // 表を作成
    magicGrid = new wxGrid(
        statusWindow,
        wxID_ANY
    );

    magicGrid->CreateGrid(0, 4);
    magicGrid->SetRowLabelSize(0);
    magicGrid->SetColLabelValue(
        0, wxString::FromUTF8("技能名")
    );
    magicGrid->SetColLabelValue(
        1, wxString::FromUTF8("レベル")
    );
    magicGrid->SetColLabelValue(
        2, wxString::FromUTF8("魔力")
    );
    magicGrid->SetColLabelValue(
        3, wxString::FromUTF8("魔法行使")
    );

    magicGrid->EnableEditing(false);

    packageDetailSizer->Add(
        magicGrid,
        0,
        wxEXPAND | wxALL,
        5
    );
}
// 魔法使い系の行追加
void Tab1Panel::AddMagicSkillRow(
    const wxString& className,
    int level,
    int magic,
    int usemagic
)
{
    EnsureMagicGrid();
    int row = magicGrid->GetNumberRows();

    magicGrid->AppendRows(1);

    magicGrid->SetCellValue(
        row, 0, className
    );
    magicGrid->SetCellValue(
        row, 1, wxString::Format("%d", level)
    );
    magicGrid->SetCellValue(
        row, 2, wxString::Format("%d", magic)
    );
    magicGrid->SetCellValue(
        row, 3, wxString::Format("%d", usemagic)
    );

    for (int col = 0; col < 4; ++col) {
        magicGrid->SetReadOnly(row, col);
    }

    statusWindow->Layout();
    statusWindow->FitInside();
    // statusPane->Layout();
    Layout();
    FitInside();
}
// 探索技能
void Tab1Panel::EnsureOtherGrid()
{
    if (otherGrid != nullptr) {
        return;
    }

    wxStaticText* title = new wxStaticText(
        statusWindow,
        wxID_ANY,
        wxString::FromUTF8("その他の技能")
    );

    packageDetailSizer->Add(
        title,
        0,
        wxALL,
        5
    );

    otherGrid = new wxGrid(
        statusWindow,
        wxID_ANY
    );

    otherGrid->CreateGrid(0, 5);
    otherGrid->SetRowLabelSize(0);
    otherGrid->SetColLabelValue(
        0, wxString::FromUTF8("技能名")
    );
    otherGrid->SetColLabelValue(
        1, wxString::FromUTF8("レベル")
    );
    otherGrid->SetColLabelValue(
        2, wxString::FromUTF8("技巧")
    );
    otherGrid->SetColLabelValue(
        3, wxString::FromUTF8("運動")
    );
    otherGrid->SetColLabelValue(
        4, wxString::FromUTF8("観察")
    );

    otherGrid->EnableEditing(false);

    packageDetailSizer->Add(
        otherGrid,
        0,
        wxEXPAND | wxALL,
        5
    );
}
// 探索技能の行追加
void Tab1Panel::AddOtherSkillRow(
    const wxString& className,
    int level,
    const wxString& techValue,
    const wxString& movValue,
    const wxString& sageValue
)
{
    EnsureOtherGrid();

    int row = otherGrid->GetNumberRows();

    otherGrid->AppendRows(1);

    otherGrid->SetCellValue(
        row, 0, className
    );

    otherGrid->SetCellValue(
        row, 1, wxString::Format("%d", level)
    );
    otherGrid->SetCellValue(
        row, 2, techValue
    );
    otherGrid->SetCellValue(
        row, 3, movValue
    );
    otherGrid->SetCellValue(
        row, 4, sageValue
    );

    for (int col = 0; col < 5; ++col) {
        otherGrid->SetReadOnly(row, col);
    }

    statusWindow->Layout();
    statusWindow->FitInside();
    // statusPane->Layout();
    Layout();
    FitInside();
}
// 行の数の計算
void Tab1Panel::ClearSkillTables()
{
    if (combatGrid != nullptr &&
        combatGrid->GetNumberRows() > 0) {
        combatGrid->DeleteRows(
            0,
            combatGrid->GetNumberRows()
        );
    }

    if (magicGrid != nullptr &&
        magicGrid->GetNumberRows() > 0) {
        magicGrid->DeleteRows(
            0,
            magicGrid->GetNumberRows()
        );
    }

    if (otherGrid != nullptr &&
        otherGrid->GetNumberRows() > 0) {
        otherGrid->DeleteRows(
            0,
            otherGrid->GetNumberRows()
        );
    }
}
// 技能判定の追加欄
void Tab1Panel::RebuildStatusDetailUI(){
    int evade = 0, magicadd = 0, weaponhit = 0, weapondamage = 0, armorevade = 0, tarmorevade = 0;
    long bonus[6]{}, lv[6]{}, hitally[6]{}, evadeally[6]{}, damageally[6]{}, magically[6]{}, usemagically[6]{}, tech[6]{}, mov[6]{}, sage[6]{};
    // weaponhitally[6]{}, weapondamageally[6]{},
    bool evadetrue = false;

    std::fill(hitally, hitally + 6, 0), std::fill(evadeally, evadeally + 6, 0), std::fill(damageally, damageally + 6, 0), std::fill(magically, magically + 6, 0);
    std::string weaponType[6];

    // 前後の半角・全角スペースを除去する関数
    auto trimSpaces = [](wxString str) -> wxString {
        str.Trim(true);   // 末尾の空白を除去
        str.Trim(false);  // 先頭の空白を除去

        // 全角スペースを除去
        while (str.StartsWith(wxString::FromUTF8("　"))) {
            str = str.Mid(1);
        }
        while (str.EndsWith(wxString::FromUTF8("　"))) {
            str.RemoveLast();
        }
        return str;
    };

    for (int i = 0; i < 6; ++i) {
		bonusTextBoxes[i]->GetValue().ToLong(&bonus[i]); // ボーナスを取得
    }

    for (int i = 0; i < 3; ++i) {
		// weaponhitTextBoxes[i]->GetValue().ToLong(&weaponhitally[i]); // 武器の命中力を取得
        // weaponaddTextBoxes[i]->GetValue().ToLong(&weapondamageally[i]); // 武器の追加Dを取得
        if (weapontypeChoices[i]->GetSelection() != wxNOT_FOUND) {
            weaponType[i] =
                weapontypeChoices[i]->GetStringSelection().ToStdString();
        }
        else {
            weaponType[i] = "";
        } // 武器種を取得
    }

    for (int i = 0; i < 3; ++i) {
    if (!armorevadeTextBoxes[i]->GetValue().ToInt(&armorevade)) {
        armorevade = 0;
    }
    tarmorevade += armorevade;
    }
    // 欄の確認
    for (int i = 0; i < std::size(talentTextBoxes); ++i){
        wxString talentName =
            talentTextBoxes[i]->GetValue().ToStdString();

        if (talentName.empty())
            continue;
    
        else if (talentName.Contains(wxString::FromUTF8("魔力強化Ⅰ"))) {
            magicadd += 1; // 魔力強化Ⅰがある場合
        }
        else if (talentName.Contains(wxString::FromUTF8("魔力強化Ⅱ"))) {
            magicadd += 2; // 魔力強化Ⅱがある場合
        }
        else if (talentName.Contains(wxString::FromUTF8("回避行動Ⅰ"))) {
            evade += 1; // 回避行動Ⅰがある場合
        }
        else if (talentName.Contains(wxString::FromUTF8("回避行動Ⅱ"))) {
            evade += 2; // 回避行動Ⅱがある場合 
        }
        else if (talentName.Contains(wxString::FromUTF8("命中強化Ⅰ"))) {
            weaponhit += 1; // 命中強化Ⅰがある場合
        }
        else if (talentName.Contains(wxString::FromUTF8("命中強化Ⅱ"))) {
            weaponhit += 2; // 命中強化Ⅱがある場合
        }
        else if (talentName.Contains(wxString::FromUTF8("武器習熟"))) {
            wxString talent = trimSpaces(wxString::FromUTF8(talentName));
            int slashPos = talent.Find('/');
                if (slashPos != wxNOT_FOUND) {
                    wxString talentType = trimSpaces(talent.Mid(slashPos + 1));

                    if (talentType == weaponType) {
                        if (talent.Contains(wxString::FromUTF8("武器習熟A"))) {
                            weapondamage += 1;
                        }
                        else if (talent.Contains(wxString::FromUTF8("武器習熟S"))) {
                            weapondamage += 2;
                        }
                    }
                }
        }
    }
    // 技能による取得
    for (int i = 0; i < 6; ++i) {
        wxString className = techTextBoxes[i]->GetValue().ToStdString(); // 技能名を取得
        if (className.empty()) {
            //this->Layout();
            continue; // 技能名が空の場合はスキップ
        }
        //　戦士系 
        else if (className == wxString::FromUTF8("ファイター") || className == wxString::FromUTF8("グラップラー") || className == wxString::FromUTF8("フェンサー")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // 技能レベル
            hitally[i] = lv[i] + bonus[0] + weaponhit; // 命中
            evadeally[i] = lv[i] + bonus[1] + evade + tarmorevade; // 回避力 
            damageally[i] = lv[i] + bonus[2] + weapondamage; // 追加D
            // 戦闘技能系の表に行を追加
            AddCombatSkillRow(
                className,
                lv[i],
                hitally[i],
                evadeally[i],
                damageally[i],
                true
            );
        } 
        // シューター
        else if (className == wxString::FromUTF8("シューター")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // 技能レベル
            hitally[i] = lv[i] + bonus[0]; // 命中力
            evadeally[i] = lv[i] + bonus[1] + evade + tarmorevade; // 回避力 
            damageally[i] = lv[i] ; //筋力Bなし
            // 回避の判定
            for (int i = 0; i < 8; ++i) { 
                wxString talentName = talentTextBoxes[i]->GetValue().ToStdString(); // 特技名を取得
                if (talentName.find(wxString::FromUTF8("射手の体術")) != wxString::npos){
                    evadetrue = true; // 射手の体術がある場合
                    break;
                }
                else {
                    evadetrue = false;
                }
            }
            // 戦闘技能系の表に行を追加
            AddCombatSkillRow(
                className,
                lv[i],
                hitally[i],
                evadeally[i],
                damageally[i],
                evadetrue
            );
        } 
        // 魔法使い系
        else if (className == wxString::FromUTF8("ソーサラー") || className == wxString::FromUTF8("コンジャラー") || className == wxString::FromUTF8("プリースト") || className == wxString::FromUTF8("マギテック") || className == wxString::FromUTF8("フェアリーテイマー")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // 魔法使い系技能
            magically[i] = lv[i] + bonus[4] + magicadd;
            usemagically[i] = magically[i]; // 行使判定の追加点のために設置(今後)
            // 魔法技能系の表に行を追加
            AddMagicSkillRow(
                wxString::FromUTF8(className),
                lv[i],
                magically[i],
                usemagically[i]
            );
        }
        // 探索技能(スカウト・レンジャー)
        else if (className == wxString::FromUTF8("スカウト") || className == wxString::FromUTF8("レンジャー")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // その他系技能
            tech[i] = lv[i] + bonus[0]; // 技巧
            mov[i] = lv[i] + bonus[1]; // 運動、先制力
            sage[i] = lv[i] + bonus[4]; //観察、知識、魔物知識
            // 探索技能系の表に行を追加
            AddOtherSkillRow(
                className,
                lv[i],
                wxString::Format("%d", tech[i]),
                wxString::Format("%d", mov[i]),
                wxString::Format("%d", sage[i])
            );
        }
        // 探索技能(セージ)
        else if (className == wxString::FromUTF8("セージ")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // その他系技能
            sage[i] = lv[i] + bonus[4]; //観察、知識、魔物知識
            // 探索技能系の表に行を追加
            AddOtherSkillRow(
                className,
                lv[i],
                wxString(""),  // 技巧は空欄
                wxString(""),  // 運動は空欄
                wxString::Format("%d", sage[i])
            );
        }
        // 探索技能(ライダー)
        else if (className == wxString::FromUTF8("ライダー")) {
            lv[i] = techlevelSpinCtrl[i]->GetValue(); // その他系技能
            mov[i] = lv[i] + bonus[1]; // 運動
            sage[i] = lv[i] + bonus[4]; // 知識
            // 探索技能系の表に行を追加
            AddOtherSkillRow(
                className,
                lv[i],
                wxString(""),
                wxString::Format("%d", mov[i]),
                wxString::Format("%d", sage[i])
            );
        }
    }   
}
// 練技などの取得欄の追加
void Tab1Panel::RebuildSkillDetailUI(){
    // 練技などの追加欄
    if (skillDetailSizer == nullptr) {
        return;
    }
    // 先頭8件は固定の戦闘特技欄。それ以降は前回生成した動的欄なので、
    // コントロールを破棄する前にベクターから古いポインタを除く。
    if (talentTextBoxes.size() > 8) talentTextBoxes.resize(8);
    if (abstTextBoxes.size() > 8) abstTextBoxes.resize(8);

    // 前回追加したSizerと、その中のコントロールを削除
    skillDetailSizer->Clear(true);
    if (extraTalentSizer != nullptr){
        extraTalentSizer->Clear(true);
    }
    if (extraAbstSizer != nullptr){
        extraAbstSizer->Clear(true);
    }
    // 破棄されたwxTextCtrlのポインタをクリア
    enhancerTextBoxes.clear();
    bardTextBoxes.clear();
    riderTextBoxes.clear();
    alchemistTextBoxes.clear();

    // 自動追加特技を管理しているなら、ここもクリア
    extraTalentTextBoxes.clear();
    extraAbstTextBoxes.clear();

    // 現在存在する技能
    std::map<wxString, int> skillLevels;

    // 技能欄を全部確認
    for (int i = 0; i < std::size(techTextBoxes); ++i){
        wxString skillName =
            techTextBoxes[i]->GetValue().ToStdString();

        if (skillName.empty())
            continue;

        skillLevels[skillName] =
            techlevelSpinCtrl[i]->GetValue();
    }

   const std::vector<wxString> skillNames = {
        "ファイター",
        "グラップラー",
        "フェンサー",
        "シューター",
        "ソーサラー",
        "コンジャラー",
        "プリースト",
        "マギテック",
        "フェアリーテイマー",
        "スカウト",
        "レンジャー",
        "セージ",
        "エンハンサー",
        "バード",
        "ライダー",
        "アルケミスト"
    };

    for (const wxString& skillName : skillNames)
    {
        auto it = skillLevels.find(skillName);

        if (it == skillLevels.end())
            continue;

        int level = it->second;

        if (skillName == "ファイター")
        {
            if (level >= 7)
                AddExtraTalent("タフネス");
        }

        if (skillName == "グラップラー")
        {
            AddExtraTalent("追加攻撃");

            if (level >= 7)
                AddExtraTalent("カウンター");
        }

        if (skillName == "ファイター" || skillName == "グラップラー")
        {
            if (level >= 13)
                AddExtraTalent("バトルマスター");
        }

        if (skillName == "ソーサラー" ||
            skillName == "コンジャラー" ||
            skillName == "プリースト" ||
            skillName == "マギテック" ||
            skillName == "フェアリーテイマー")
        {
            if (level >= 11)
                AddExtraTalent("ルーンマスター");
        }

        if (skillName == "スカウト")
        {
            if (level >= 5)
                AddExtraTalent("トレジャーハント");

            if (level >= 7)
                AddExtraTalent("ファストアクション");

            if (level >= 9)
                AddExtraTalent("影走り");

            if (level >= 12)
                AddExtraTalent("トレジャーマスター");

            if (level >= 15)
                AddExtraTalent("匠の技");
        }

        if (skillName == "レンジャー")
        {
            if (level >= 5)
                AddExtraTalent("サバイバビリティ");

            if (level >= 7)
                AddExtraTalent("不屈");

            if (level >= 9)
                AddExtraTalent("ポーションマスター");

            if (level >= 12)
                AddExtraTalent("縮地");

            if (level >= 15)
                AddExtraTalent("ランアンドガン");
        }

        if (skillName == "セージ")
        {
            if (level >= 5)
                AddExtraTalent("鋭い目");

            if (level >= 7)
                AddExtraTalent("弱点看破");

            if (level >= 9)
                AddExtraTalent("マナセーブ");

            if (level >= 12)
                AddExtraTalent("マナ耐性");

            if (level >= 15)
                AddExtraTalent("賢人の知恵");
        }
    
           
        if (skillName == "エンハンサー")
        {
            wxStaticBoxSizer* enhancerSizer =
                new wxStaticBoxSizer(
                    wxVERTICAL,
                    skillWindow,
                    wxString::FromUTF8("練技")
                );

            for (int i = 0; i < level; ++i)
            {
                wxTextCtrl* textBox =
                    new wxTextCtrl(
                        skillWindow,
                        wxID_ANY,
                        "",
                        wxDefaultPosition,
                        wxSize(120, -1)
                    );

                enhancerTextBoxes.push_back(textBox);

                enhancerSizer->Add(
                    textBox,
                    0,
                    wxEXPAND | wxALL,
                    2
                );
            }

            skillDetailSizer->Add(
                enhancerSizer,
                0,
                wxEXPAND | wxALL,
                5
            );
        }

        else if (skillName == "バード")
        {
            wxStaticBoxSizer* bardSizer =
                new wxStaticBoxSizer(
                    wxVERTICAL,
                    skillWindow,
                    wxString::FromUTF8("呪歌・終律")
                );

            for (int i = 0; i < level; ++i)
            {
                wxTextCtrl* textBox =
                    new wxTextCtrl(
                        skillWindow,
                        wxID_ANY,
                        "",
                        wxDefaultPosition,
                        wxSize(120, -1)
                    );

                bardTextBoxes.push_back(textBox);

                bardSizer->Add(
                    textBox,
                    0,
                    wxEXPAND | wxALL,
                    2
                );
            }

            skillDetailSizer->Add(
                bardSizer,
                0,
                wxEXPAND | wxALL,
                5
            );
        }

        else if (skillName == "ライダー")
        {
            wxStaticBoxSizer* riderSizer =
                new wxStaticBoxSizer(
                    wxVERTICAL,
                    skillWindow,
                    wxString::FromUTF8("騎芸")
                );

            for (int i = 0; i < level; ++i)
            {
                wxTextCtrl* textBox =
                    new wxTextCtrl(
                        skillWindow,
                        wxID_ANY,
                        "",
                        wxDefaultPosition,
                        wxSize(120, -1)
                    );

                riderTextBoxes.push_back(textBox);

                riderSizer->Add(
                    textBox,
                    0,
                    wxEXPAND | wxALL,
                    2
                );
            }

            skillDetailSizer->Add(
                riderSizer,
                0,
                wxEXPAND | wxALL,
                5
            );
        }

        else if (skillName == "アルケミスト")
        {
            wxStaticBoxSizer* alchemistSizer =
                new wxStaticBoxSizer(
                    wxVERTICAL,
                    skillWindow,
                    wxString::FromUTF8("賦術")
                );

            for (int i = 0; i < level; ++i)
            {
                wxTextCtrl* textBox =
                    new wxTextCtrl(
                        skillWindow,
                        wxID_ANY,
                        "",
                        wxDefaultPosition,
                        wxSize(120, -1)
                    );

                alchemistTextBoxes.push_back(textBox);

                alchemistSizer->Add(
                    textBox,
                    0,
                    wxEXPAND | wxALL,
                    2
                );
            }

            skillDetailSizer->Add(
                alchemistSizer,
                0,
                wxEXPAND | wxALL,
                5
            );
        }
    }

    // レイアウト更新
    // ->Layout();
    skillWindow->Layout();

    // Tab1PanelがwxScrolledWindowなら
    FitInside();
}
// 自動取得技能の追加関数
void Tab1Panel::AddExtraTalent(const wxString& talentName)
{
    // 特技名
    wxString talentText =
        wxString::Format(
            wxString::FromUTF8("自: %s"),
            talentName
        );

    wxTextCtrl* talentBox =
        new wxTextCtrl(
            skillWindow,
            wxID_ANY,
            talentText,
            wxDefaultPosition,
            wxSize(120, -1)
        );

    // 効果概要
    wxTextCtrl* abstBox =
        new wxTextCtrl(
            skillWindow,
            wxID_ANY,
            "",
            wxDefaultPosition,
            wxSize(120, -1)
        );

    // vectorに追加
    talentTextBoxes.push_back(talentBox);
    abstTextBoxes.push_back(abstBox);

    // Sizerに追加
    extraTalentSizer->Add(
        talentBox,
        0,
        wxALIGN_LEFT
    );

    extraAbstSizer->Add(
        abstBox,
        0,
        wxALIGN_LEFT
    );

    // 必要ならイベントも登録
    talentBox->Bind(
        wxEVT_TEXT,
        &Tab1Panel::Updatehp,
        this
    );

    // レイアウト更新
    skillWindow->Layout();
    skillWindow->FitInside();
}
// HPなどの更新関数
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
// 使用経験値の更新
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
// スピンボタンの変更の変更時イベント
void Tab1Panel::OnSpinCtrlChange(wxCommandEvent& event) {
    Updateexp(event);   // 1つ目の処理
    Updatelevel(event); // 2つ目の処理
    ClearSkillTables(); // 行数の確認処理
    RebuildSkillDetailUI(); //4つ目の処理 
    RebuildStatusDetailUI(); //4つ目の処理 
	Updatehp(event); // 3つ目の処理
}
// ボーナスとHPの一括更新
void Tab1Panel::abilityChange(wxCommandEvent& event) {
    Updatebonus(event);   // 2つ目の処理
    Updatehp(event); // 3つ目の処理
}
// tab3からの計算結果の反映→経験点・所持金・名誉点
void Tab1Panel::SetSessionTotals(
    long long experience,
    long long money,
    long long honor
)
{
    getexpSpinCtrl->SetValue(
        static_cast<int>(experience)
    );

    poketSpinCtrl->SetValue(
        static_cast<int>(money)
    );

    glorySpinCtrl->SetValue(
        static_cast<int>(honor)
    );

    wxCommandEvent event(wxEVT_TEXT, getexpSpinCtrl->GetId());
    event.SetEventObject(getexpSpinCtrl);
    OnCompareValues(event);
}
// tab3からの計算結果の反映→成長
void Tab1Panel::SetGrowthTotals(
    const std::array<long long, 6>& growthTotals)
{
    for (int i = 0; i < 6; ++i) {
        growthSpinCtrl[i]->SetValue(
            static_cast<int>(growthTotals[i])
        );
    }
    // ボーナスとHPの更新処理を呼び出す
    wxCommandEvent event(wxEVT_TEXT, growthSpinCtrl[0]->GetId());
    event.SetEventObject(growthSpinCtrl[0]);
    UpdateAttribute(event); // 1つ目の処理
    abilityChange(event);
}
// Tab3からの計算結果の反映 → マテリアルカード
void Tab1Panel::SetMatecaTotals(
    const std::array<std::array<long long, 4>, 5>& matecaTotals
)
{
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 4; ++col) {
            matecaSpinCtrl[row][col]->SetValue(
                static_cast<int>(matecaTotals[row][col])
            );
        }
    }
}
void Tab1Panel::OnExpandAll(wxCommandEvent& event)
{
    for (wxCollapsiblePane* pane : collapsiblePanes) {
        pane->Expand();
    }

    Layout();
    FitInside();
}

void Tab1Panel::OnCollapseAll(wxCommandEvent& event)
{
    for (wxCollapsiblePane* pane : collapsiblePanes) {
        pane->Collapse();
    }

    Layout();
    FitInside();
}

// json rootの作成
json Tab1Panel::CreateJsonFromGUI(){

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
    root["rankSelection"] = rankChoices->GetSelection();

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

    // HP、MP、抵抗力、命中力
    for (int i = 0; i < 4; ++i) {
        root["hp"][i] = wxAtoi(hpTextBoxes[i]->GetValue());  // 数値として保存
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

    for (wxTextCtrl* textBox : enhancerTextBoxes)
    {
        root["enhancer"].push_back(
            textBox->GetValue().ToStdString()
        );
    }

    for (wxTextCtrl* textBox : bardTextBoxes)
    {
        root["bard"].push_back(
            textBox->GetValue().ToStdString()
        );
    }

    for (wxTextCtrl* textBox : riderTextBoxes)
    {
        root["rider"].push_back(
            textBox->GetValue().ToStdString()
        );
    }

    for (wxTextCtrl* textBox : alchemistTextBoxes)
    {
        root["alchemist"].push_back(
            textBox->GetValue().ToStdString()
        );
    }

    // 戦闘特技、効果概要
    json talentArray = json::array();

    for (size_t i = 0; i < talentTextBoxes.size(); ++i)
    {
        json talent;

        talent["name"] =
            talentTextBoxes[i]->GetValue().ToStdString();

        talent["description"] =
            abstTextBoxes[i]->GetValue().ToStdString();

        talentArray.push_back(talent);
    }

    root["talents"] = talentArray;

    root["freetalent"] = std::string(freetalentTextBox->GetValue().ToUTF8().data()); // 自動習得特技
    root["freeabst"] = std::string(freeabstTextBox->GetValue().ToUTF8().data()); // 自動習得効果
    root["magictalent"] = std::string(magictalentTextBox->GetValue().ToUTF8().data()); // よく使う魔法など
    root["magicabst"] = std::string(magicabstTextBox->GetValue().ToUTF8().data()); // MP、効果概要
    root["battle"] = std::string(battleTextBox->GetValue().ToUTF8().data()); // 戦闘効果メモ
    root["other"] = std::string(otherTextBox->GetValue().ToUTF8().data()); // 経歴、その他メモ
    root["chat"] = std::string(chatTextBox->GetValue().ToUTF8().data()); // チャットパレット

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
    root["armor"] = armorSpinCtrl->GetValue();

    // 選択式入力
    root["weaponType"] = json::array();
    root["armorType"] = json::array();
    for (int i = 0; i < 3; ++i) {
        root["weaponType"].push_back(weapontypeChoices[i]->GetSelection());
        root["armorType"].push_back(armortypeChoices[i]->GetSelection());
    }

    // マテリアルカード、ポーション、指輪・腕輪
    root["mateca"] = json::array();
    for (int row = 0; row < 5; ++row) {
        json values = json::array();
        for (int col = 0; col < 4; ++col)
            values.push_back(matecaSpinCtrl[row][col]->GetValue());
        root["mateca"].push_back(values);
    }
    root["potions"] = json::array();
    root["rings"] = json::array();
    for (int i = 0; i < 6; ++i) {
        root["potions"].push_back({{"name", potionTextBoxes[i]->GetValue().ToStdString()}, {"count", potionSpinCtrl[i]->GetValue()}});
        root["rings"].push_back({{"name", ringTextBoxes[i]->GetValue().ToStdString()}, {"count", ringSpinCtrl[i]->GetValue()}});
    }

    // 技能名・レベルに応じて動的に生成される入力欄
    auto saveDynamicTextBoxes = [](const std::vector<wxTextCtrl*>& boxes) {
        json values = json::array();
        for (const wxTextCtrl* box : boxes)
            values.push_back(box ? box->GetValue().ToStdString() : std::string());
        return values;
    };
    root["dynamicSkills"] = {
        {"enhancer", saveDynamicTextBoxes(enhancerTextBoxes)},
        {"bard", saveDynamicTextBoxes(bardTextBoxes)},
        {"rider", saveDynamicTextBoxes(riderTextBoxes)},
        {"alchemist", saveDynamicTextBoxes(alchemistTextBoxes)}
    };

    return root;
}

// jsonファイルへの書き出し処理
bool Tab1Panel::SaveToJson(const wxString& filename) {
    json root = CreateJsonFromGUI();
    
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
    try {
        file >> root; // JSONパース
    }
    catch (const std::exception& e) {
        wxLogError("Failed to parse JSON: %s", e.what());
        return false;
    }
    file.close();

    // アプリ全体の保存形式なら Tab1 部分を取り出す
    if (root.is_object() && root.contains("tab1") && root["tab1"].is_object())
        root = root["tab1"];

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
    if (root.contains("rankSelection") && root["rankSelection"].is_number_integer())
        rankChoices->SetSelection(std::clamp(root["rankSelection"].get<int>(), -1, static_cast<int>(rankChoices->GetCount()) - 1));

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

    // 技能レベルに応じて追加入力欄を再構築する。
    RebuildSkillDetailUI();

    // 戦闘特技・効果概要（固定8欄＋動的に追加された欄）
    if (root.contains("talents") && root["talents"].is_array()) {
        const auto& savedTalents = root["talents"];
        const std::size_t count = std::min(savedTalents.size(), talentTextBoxes.size());
        for (std::size_t i = 0; i < count; ++i) {
            if (!savedTalents[i].is_object()) continue;
            if (savedTalents[i].contains("name") && savedTalents[i]["name"].is_string())
                talentTextBoxes[i]->ChangeValue(wxString::FromUTF8(savedTalents[i]["name"].get<std::string>().c_str()));
            if (savedTalents[i].contains("description") && savedTalents[i]["description"].is_string())
                abstTextBoxes[i]->ChangeValue(wxString::FromUTF8(savedTalents[i]["description"].get<std::string>().c_str()));
        }
    } else {
        for (int i = 0; i < 8; ++i) {
            if (root.contains("talent") && root["talent"].is_array() && i < root["talent"].size())
                talentTextBoxes[i]->ChangeValue(wxString::FromUTF8(root["talent"][i].get<std::string>().c_str()));
            if (root.contains("abst") && root["abst"].is_array() && i < root["abst"].size())
                abstTextBoxes[i]->ChangeValue(wxString::FromUTF8(root["abst"][i].get<std::string>().c_str()));
        }
    }

    freetalentTextBox->SetValue(wxString::FromUTF8(root["freetalent"].get<std::string>().c_str()));
    freeabstTextBox->SetValue(wxString::FromUTF8(root["freeabst"].get<std::string>().c_str()));
    magictalentTextBox->SetValue(wxString::FromUTF8(root["magictalent"].get<std::string>().c_str()));
    magicabstTextBox->SetValue(wxString::FromUTF8(root["magicabst"].get<std::string>().c_str()));
    battleTextBox->SetValue(wxString::FromUTF8(root["battle"].get<std::string>().c_str()));
    otherTextBox->SetValue(wxString::FromUTF8(root["other"].get<std::string>().c_str()));
    chatTextBox->SetValue(wxString::FromUTF8(root["chat"].get<std::string>().c_str()));

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

    if (root.contains("weaponType") && root["weaponType"].is_array())
        for (int i = 0; i < 3 && i < root["weaponType"].size(); ++i)
            if (root["weaponType"][i].is_number_integer()) weapontypeChoices[i]->SetSelection(std::clamp(root["weaponType"][i].get<int>(), -1, static_cast<int>(weapontypeChoices[i]->GetCount()) - 1));
    if (root.contains("armorType") && root["armorType"].is_array())
        for (int i = 0; i < 3 && i < root["armorType"].size(); ++i)
            if (root["armorType"][i].is_number_integer()) armortypeChoices[i]->SetSelection(std::clamp(root["armorType"][i].get<int>(), -1, static_cast<int>(armortypeChoices[i]->GetCount()) - 1));
    if (root.contains("armor") && root["armor"].is_number_integer()) armorSpinCtrl->SetValue(root["armor"].get<int>());

    if (root.contains("mateca") && root["mateca"].is_array())
        for (int row = 0; row < 5 && row < root["mateca"].size(); ++row)
            for (int col = 0; col < 4 && root["mateca"][row].is_array() && col < root["mateca"][row].size(); ++col)
                if (root["mateca"][row][col].is_number_integer()) matecaSpinCtrl[row][col]->SetValue(root["mateca"][row][col].get<int>());

    auto restoreInventory = [&](const char* key, wxTextCtrl** names, wxSpinCtrl** counts) {
        if (!root.contains(key) || !root[key].is_array()) return;
        for (int i = 0; i < 6 && i < root[key].size(); ++i) {
            const auto& item = root[key][i];
            if (!item.is_object()) continue;
            if (item.contains("name") && item["name"].is_string()) names[i]->ChangeValue(wxString::FromUTF8(item["name"].get<std::string>().c_str()));
            if (item.contains("count") && item["count"].is_number_integer()) counts[i]->SetValue(item["count"].get<int>());
        }
    };
    restoreInventory("potions", potionTextBoxes, potionSpinCtrl);
    restoreInventory("rings", ringTextBoxes, ringSpinCtrl);

    auto restoreDynamicTextBoxes = [&](const char* key, std::vector<wxTextCtrl*>& boxes) {
        const nlohmann::json* values = nullptr;
        if (root.contains("dynamicSkills") && root["dynamicSkills"].is_object() &&
            root["dynamicSkills"].contains(key) && root["dynamicSkills"][key].is_array()) {
            values = &root["dynamicSkills"][key];
        } else if (root.contains(key) && root[key].is_array()) {
            // 旧版 Tab1 JSON は enhancer / bard / rider / alchemist をトップレベルに保存していた。
            values = &root[key];
        }
        if (values == nullptr) return;
        for (std::size_t i = 0; i < boxes.size() && i < values->size(); ++i)
            if ((*values)[i].is_string()) boxes[i]->ChangeValue(wxString::FromUTF8((*values)[i].get<std::string>().c_str()));
    };
    restoreDynamicTextBoxes("enhancer", enhancerTextBoxes);
    restoreDynamicTextBoxes("bard", bardTextBoxes);
    restoreDynamicTextBoxes("rider", riderTextBoxes);
    restoreDynamicTextBoxes("alchemist", alchemistTextBoxes);

    // 技能テーブルは保存値ではなく、復元した入力欄から再生成する。
    ClearSkillTables();
    RebuildStatusDetailUI();
    Layout();
    FitInside();
    return true;
}

bool Tab1Panel::CopyToCocofolia(){
    
    json root = CreateJsonFromGUI();

    json cocofolia;

    cocofolia["kind"] = "character";

    json& data = cocofolia["data"];

    // プレイヤー名
    data["playerName"] = root["PL"];
    // キャラクター名
    data["name"] = root["name"];

    // ステータス
    data["status"] = json::array();

    data["status"].push_back(
        {{"label", "HP"},
        {"value", std::to_string(root["hp"][0].get<int>())},
        {"max",   std::to_string(root["hp"][0].get<int>())}}
    );
    data["status"].push_back(
        {{"label", "MP"},
        {"value", std::to_string(root["hp"][1].get<int>())},
        {"max",   std::to_string(root["hp"][1].get<int>())}}
    );
    data["status"].push_back(
        {{"label", wxString::FromUTF8("防護点")},
        {"value", std::to_string(root["armor"].get<int>())},
        {"max",   std::to_string(root["armor"].get<int>())}}
    );
    // root["tech"] に登録された技能を確認
    bool hasEnhancer = false;
    bool hasBard = false;
    bool hasAlchemist = false;

    for (const auto& tech : root["tech"]) {
        if (!tech.is_string()) {
            continue;
        }

        const std::string name = tech.get<std::string>();

        if (name == "エンハンサー") {
            hasEnhancer = true;
        }
        else if (name == "バード") {
            hasBard = true;
        }
        else if (name == "アルケミスト") {
            hasAlchemist = true;
        }
    }
    // エンハンサー：練技効果時間
    if (hasEnhancer) {
        data["status"].push_back({
            {"label", "練技効果時間"},
            {"value", "0"},
            {"max", "0"}
        });
    }
    // バード：3項目
    if (hasBard) {
        data["status"].push_back({
            {"label", "⤴︎"},
            {"value", "0"},
            {"max", "0"}
        });

        data["status"].push_back({
            {"label", "⤵︎"},
            {"value", "0"},
            {"max", "0"}
        });

        data["status"].push_back({
            {"label", "♡"},
            {"value", "0"},
            {"max", "0"}
        });
    }
    // アルケミスト：賦術効果時間
    if (hasAlchemist) {
        data["status"].push_back({
            {"label", "賦術効果時間"},
            {"value", "0"},
            {"max", "0"}
        });
    }

    // イニシアチブ
    data["initiative"] = root["move"];

    // パラメータ
    data["params"] = json::array();
    // 冒険者レベル
    data["params"].push_back({
        {"label", "冒険者レベル"},
        {"value", levelTextBox->GetValue().ToStdString()}
    });
    // 能力値ボーナス
    const char* bonusLabels[] = {
        "器用度B", "敏捷度B", "筋力B",
        "生命力B", "知力B", "精神力B"
    };
    for (int i = 0; i < 6; ++i) {
        data["params"].push_back({
            {"label", bonusLabels[i]},
            {"value", bonusTextBoxes[i]->GetValue().ToStdString()}
        });
    }
    // 技能名・技能レベル
    for (int i = 0; i < 6; ++i) {
        wxString techName = techTextBoxes[i]->GetValue().Trim();

        if (techName.IsEmpty()) {
            continue;
        }

        data["params"].push_back({
            {"label", techName.ToStdString()},
            {"value", std::to_string(techlevelSpinCtrl[i]->GetValue())}
        });
    }
    // その他
    data["faces"] = json::array();

    data["x"] = 0;
    data["y"] = 0;
    data["z"] = 0;
    data["angle"] = 0;

    data["width"] = 4;
    data["height"] = 4;

    data["active"] = true;
    data["secret"] = false;
    data["invisible"] = false;
    data["hideStatus"] = false;

    data["color"] = "";
    data["roomId"] = nullptr;

    // コマンド
    std::string commands = root["chat"].get<std::string>();

    // 先頭のチャットパレットの後ろに改行を追加
    if (!commands.empty()) {
        commands += "\n";
    }
    // 以下に判定コマンドを追加する
    commands += std::string("2d") + " " + "平目\n";

    const char* resistLabels[] = {
        "生命", "精神"
    };
    for (int i = 0; i < 2; ++i) {
        int resistbonus = root["hp"][i+2].get<int>();
        commands += std::string("2d+") + std::to_string(resistbonus) + " " + resistLabels[i] + "判定\n";
    } 

    for (const auto& tech : root["tech"]) {
        if (!tech.is_string()) {
            continue;
        }

        const std::string name = tech.get<std::string>();

        if (name == "スカウト") {
            commands += std::string("2d+{スカウト}+{器用度B}") + " " + name + "技巧判定\n";
            commands += std::string("2d+{スカウト}+{敏捷度B}") + " " + name + "運動判定\n";
            commands += std::string("2d+{スカウト}+{知力B}") + " " + name + "観察判定\n";
        }
        else if (name == "レンジャー") {
            commands += std::string("2d+{レンジャー}+{器用度B}") + " " + name + "技巧判定\n";
            commands += std::string("2d+{レンジャー}+{敏捷度B}") + " " + name + "運動判定\n";
            commands += std::string("2d+{レンジャー}+{知力B}") + " " + name + "観察判定\n";
        }
        else if (name == "セージ") {
            commands += std::string("2d+{セージ}+{知力B}") + " " + name +  "知識判定\n";
        }
        else if (name == "バード") {
            commands += std::string("2d+{バード}+{知力B}") + " " + name +  "知識判定\n";
            commands += std::string("2d+{バード}+{精神力B}") + " " + name +  "演奏判定\n";
        }
        else if (name == "ライダー") {
            commands += std::string("2d+{ライダー}+{敏捷度B}") + " " + name +  "運動判定\n";
            commands += std::string("2d+{ライダー}+{知力B}") + " " + name +  "知識判定\n";
            commands += std::string("2d+{ライダー}+{知力B}") + " " + name +  "観察判定\n";
        }
        else if (name == "アルケミスト") {
            commands += std::string("2d+{アルケミスト}+{知力B}") + " " + name +  "知識判定\n";
            commands += std::string("2d+{アルケミスト}+{知力B}") + " " + name +  "賦術判定\n";
        }

        else if (name == "ファイター" || name == "グラップラー" || name == "フェンサー") {
            commands += std::string("2d+{") + name + "}+{器用度B}"+ " " + name +  "命中判定\n";
            commands += std::string("2d+{") + name + "}+{敏捷度B}"+ " " + name +  "回避判定\n";
            // commands += "K" + root["weapondm"] + "+{" + name + "}+{筋力B}" + " " + name +  "ダメージ判定/\n";
        }
        else if (name == "シューター") {
            commands += std::string("2d+{") + name + "}+{器用度B}"+ " " + name +  "命中判定\n";
            commands += std::string("2d") + " " + name + "回避判定\n";
            // commands += ;
        }
        else if (name == "ソーサラー"  || name == "コンジャラー" || name == "プリースト" || name == "マギテック" || name == "フェアリーテイマー") {
            commands += std::string("2d+{") + name + "}+{知力B}"+ " " + name +  "魔法行使判定\n";
            // commands += ;
        }
    }
    data["commands"] = commands;

    // キャラクター名
    data["speaking"] = true;
    
    // メモ
    std::string memonote = "PL: " + root["PL"].get<std::string>() + "\n";

    memonote += root["species"].get<std::string>() + "/";
    memonote += root["sex"].get<std::string>() + "/";
    memonote += std::to_string(root["age"].get<int>()) + "\n";

    memonote += "穢れ: ";
    memonote += std::to_string(root["impurity"].get<int>());
    memonote += " 信仰: " + root["faith"].get<std::string>() + "\n";

    memonote += "ランク: ";
    memonote += std::to_string(root["rankSelection"].get<int>()) + "\n";

    data["memo"] = memonote;

    // -------------------------
    // JSON文字列を作成
    // -------------------------
    std::string output = cocofolia.dump();

    // -------------------------
    // クリップボードへコピー
    // -------------------------
    if (!wxTheClipboard->Open()) {
        wxLogError("Failed to open clipboard.");
        return false;
    }

    wxTheClipboard->SetData(
        new wxTextDataObject(
            wxString::FromUTF8(output.c_str())
        )
    );

    wxTheClipboard->Close();

    return true;
}