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


// コンストラクタ
Tab2Panel::Tab2Panel(wxWindow* parent, wxNotebook* notebook, Tab1Panel* tab1Panel) : wxPanel(parent), notebook(notebook), tab1Panel(tab1Panel), resultsSizer(new wxBoxSizer(wxVERTICAL)) {
    // メインレイアウト

    // コンストラクタなどで一度だけ呼び出す
    //new wxLogWindow(this, wxString::FromUTF8("デバッグログ"), true);

    // フォント設定
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 名前入力セクション
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* nameLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("名前: "));
    nameTextBox = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8("新規キャラクター"), wxDefaultPosition, wxSize(100, -1));
    nameErrorText = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    nameErrorText->SetForegroundColour(*wxRED);
    nameErrorText->Hide();

    nameSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    nameSizer->Add(nameTextBox, 1);

    mainSizer->Add(nameSizer, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(nameErrorText, 0, wxLEFT | wxRIGHT, 10);

    // 種族リストボックスセクション
    wxBoxSizer* speciesSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* speciesLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("種族: "));
    wxArrayString speciesChoices;
    speciesChoices.Add(wxString::FromUTF8("選択してください"));
    speciesChoices.Add(wxString::FromUTF8("人間"));
    speciesChoices.Add(wxString::FromUTF8("エルフ"));
    speciesChoices.Add(wxString::FromUTF8("ドワーフ"));
    speciesChoices.Add(wxString::FromUTF8("タビット"));
    speciesChoices.Add(wxString::FromUTF8("ルーンフォーク"));
    speciesChoices.Add(wxString::FromUTF8("ナイトメア"));
    speciesChoices.Add(wxString::FromUTF8("リカント"));
    speciesChoices.Add(wxString::FromUTF8("リルドラケン"));
    speciesChoices.Add(wxString::FromUTF8("グラスランナー"));
    speciesChoices.Add(wxString::FromUTF8("メリア"));
    speciesChoices.Add(wxString::FromUTF8("ティエンス"));
    speciesChoices.Add(wxString::FromUTF8("レプラカーン"));

    speciesListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(200, 100), speciesChoices, wxLB_SINGLE);
    speciesListBox->Bind(wxEVT_LISTBOX, &Tab2Panel::OnSpeciesSelected, this);
    speciesListBox->SetSelection(0);
    

    InitializeSpeciesData(); // グリッド生成に必要なデータを先に初期化


    speciesSizer->Add(speciesLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    speciesSizer->Add(speciesListBox, 1);

    mainSizer->Add(speciesSizer, 0, wxEXPAND | wxALL, 10);

    // 生まれセクション
    wxFont font(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxBoxSizer* birthSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* birthLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("生まれ: "));
    wxStaticText* skillLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("技: "));
    skillSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    skillSpinCtrl->SetFont(font);  // フォント適用
    wxStaticText* bodyLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("体: "));
    bodySpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    bodySpinCtrl->SetFont(font);  // フォント適用
    wxStaticText* mindLabel = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("心: "));
    mindSpinCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 99, 7);
    mindSpinCtrl->SetFont(font);  // フォント適用

    birthSizer->Add(birthLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(skillLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(skillSpinCtrl, 0, wxRIGHT, 10);
    birthSizer->Add(bodyLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(bodySpinCtrl, 0, wxRIGHT, 10);
    birthSizer->Add(mindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    birthSizer->Add(mindSpinCtrl, 0);

    mainSizer->Add(birthSizer, 0, wxEXPAND | wxALL, 10);

    // 作成ボタン
    wxButton* createButton = new wxButton(this, wxID_ANY, wxString::FromUTF8("作成"));
    createButton->Bind(wxEVT_BUTTON, &Tab2Panel::OnCreateButtonClicked, this);

    mainSizer->Add(createButton, 0, wxALIGN_CENTER | wxALL, 10);




    // 表示専用の種族表コンテナ
    wxStaticBoxSizer* gridBox = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("種族の生まれ表"));

    wxPanel* gridPanel = new wxPanel(this);
    wxBoxSizer* gridPanelSizer = new wxBoxSizer(wxVERTICAL);

    // 表示用のグリッド作成
    for (const auto& [name, data] : speciesData) {
        int numRows = static_cast<int>(data.size());
        int numCols = 4; // 生まれ、技能、能力値、経験点

        wxGrid* grid = new wxGrid(gridPanel, wxID_ANY);
        grid->SetMinSize(wxSize(500, 300));
        grid->CreateGrid(numRows, numCols);

        // カラムラベル
        grid->SetColLabelValue(0, wxString::FromUTF8("生まれ"));
        grid->SetColLabelValue(1, wxString::FromUTF8("初期所有技能"));
        grid->SetColLabelValue(2, wxString::FromUTF8("基礎能力値"));
        grid->SetColLabelValue(3, wxString::FromUTF8("初期経験点"));

        grid->SetRowLabelSize(0); // 行ラベル非表示

        // 各セルにデータをセット
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                if (col < static_cast<int>(data[row].size())) {
                    grid->SetCellValue(row, col, data[row][col]);
                    grid->SetReadOnly(row, col);  // 編集不可にする
                }
            }
        }

        // セルの自動サイズ調整
        grid->AutoSizeColumns();
        // 必要に応じて行の高さも自動調整
        grid->AutoSizeRows();
        grid->Hide(); // 初期は隠す
        speciesTables[name] = grid;
        gridPanelSizer->Add(grid, 1, wxEXPAND | wxALL, 5);
    }

    gridPanel->SetSizer(gridPanelSizer);
    gridBox->Add(gridPanel, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(gridBox, 1, wxEXPAND | wxALL, 10);






    // レイアウトを適用
    this->SetSizer(mainSizer);
}
// デストラクタ
Tab2Panel::~Tab2Panel() {
    // resultsSizer を解放
    delete resultsSizer;
}


void Tab2Panel::InitializeSpeciesData()
{
    // 例：「人間」のデータを入れる
    speciesData[wxString::FromUTF8("人間")] = {
        // ルルブ1
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技8    体4    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技6    体5    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("スカウト&フェンサー"), wxString::FromUTF8("技10    体7    心4"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("一般人"), wxString::FromUTF8("なし"), wxString::FromUTF8("技7    体7    心7"), wxString::FromUTF8("3000点")},
        {wxString::FromUTF8("傭兵"), wxString::FromUTF8("ファイターorグラップラー"), wxString::FromUTF8("技8    体10    心4"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技4    体8    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技7    体4    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("冒険者"), wxString::FromUTF8("なし"), wxString::FromUTF8("技2d    体2d    心2d"), wxString::FromUTF8("3000点")},
        //ルルブ2
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技9    体5    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("剣士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技9    体6    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技8    体8    心5"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイター"), wxString::FromUTF8("技7    体9    心5"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技6    体8    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("歌人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技5    体7    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技5    体6    心10"), wxString::FromUTF8("2000点")}
    };
    // エルフ
    speciesData[wxString::FromUTF8("エルフ")] = {
        {wxString::FromUTF8("剣士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技12    体5    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("薬師"), wxString::FromUTF8("セージ&レンジャー"), wxString::FromUTF8("技10    体5    心11"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技9    体5    心12"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技9    体4    心13"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技10    体3    心13"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技13    体5    心8"), wxString::FromUTF8("2500点")},

        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技11    体5    心10"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("密偵"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技12    体4    心10"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技10    体2    心14"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技11    体4    心11"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技9    体3    心14"), wxString::FromUTF8("2500点")}
    };
    // ドワーフ
    speciesData[wxString::FromUTF8("ドワーフ")] = {
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シュター"), wxString::FromUTF8("技6    体8    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイター"), wxString::FromUTF8("技4    体11    心5"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技5    体10    心5"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技4    体7    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技6    体7    心7"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技3    体8    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技6    体9    心5"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技5    体9    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技4    体8    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技5    体6    心9"), wxString::FromUTF8("2000点")}
    };
    // タビット
    speciesData[wxString::FromUTF8("タビット")] = {
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技6    体6    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技5    体7    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技5    体8    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技8    体5    心9"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("魔導師"), wxString::FromUTF8("ソーサラー&コンジャラー"), wxString::FromUTF8("技5    体6    心11"), wxString::FromUTF8("1000点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技7    体5    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技6    体7    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("薬師"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技7    体6    心9"), wxString::FromUTF8("2500点")}
    };
    // ルーンフォーク
    speciesData[wxString::FromUTF8("ルーンフォーク")] = {
        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技8    体10    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技12    体8    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイターorグラップラー"), wxString::FromUTF8("技9    体12    心5"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技12    体8    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技9    体8    心9"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("密偵"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技12    体6    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技10    体8    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技11    体9    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技8    体9    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技7    体9    心10"), wxString::FromUTF8("2000点")}
    };
    // ナイトメア
    speciesData[wxString::FromUTF8("ナイトメア")] = {
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技5    体13    心12"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("傭兵"), wxString::FromUTF8("ファイターorグラップラー"), wxString::FromUTF8("技7    体15    心8"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー&スカウト"), wxString::FromUTF8("技11    体13    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技6    体14    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技9    体9    心12"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技8    体13    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技9    体14    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技10    体10    心10"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技9    体12    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技6    体11    心13"), wxString::FromUTF8("2000点")}
    };
    // リカント
    speciesData[wxString::FromUTF8("リカント")] = {
        {wxString::FromUTF8("密偵"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技13    体5    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイター"), wxString::FromUTF8("技10    体9    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技11    体7    心7"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技12    体6    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技9    体8    心8"), wxString::FromUTF8("2500点")},

        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技8    体9    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技11    体8    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技10    体8    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技10    体7    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技9    体7    心9"), wxString::FromUTF8("2000点")}
    };
    // リルドラケン
    speciesData[wxString::FromUTF8("リルドラケン")] = {
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技6    体12    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技6    体13    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイター"), wxString::FromUTF8("技5    体14    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("商人"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技5    体11    心9"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技4    体13    心8"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技7    体12    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技6    体11    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技5    体12    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("魔法使い"), wxString::FromUTF8("ソーサラーorコンジャラー"), wxString::FromUTF8("技4    体12    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技3    体12    心10"), wxString::FromUTF8("2000点")}
    };
    // グラスランナー
    speciesData[wxString::FromUTF8("グラスランナー")] = {
        {wxString::FromUTF8("盗っ人"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技13    体0    心12"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技14    体1    心10"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技12    体1    心12"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技14    体0    心11"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("趣味人"), wxString::FromUTF8("セージorバード"), wxString::FromUTF8("技12    体0    心13"), wxString::FromUTF8("2500点")},

        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技11    体1    心13"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技14    体2    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("旅人"), wxString::FromUTF8("なし"), wxString::FromUTF8("技11    体2    心12"), wxString::FromUTF8("3000点")},
        {wxString::FromUTF8("密偵"), wxString::FromUTF8("フェンサー&スカウト"), wxString::FromUTF8("技15    体0    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技12    体0    心13"), wxString::FromUTF8("2500点")}
    };
    // メリア
    speciesData[wxString::FromUTF8("メリア")] = {
        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技9    体8    心12"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技8    体8    心13"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技8    体7    心14"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔術師"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技8    体6    心15"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("操霊術師"), wxString::FromUTF8("コンジャラー"), wxString::FromUTF8("技7    体6    心16"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技10    体7    心12"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技10    体8    心11"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("魔導師"), wxString::FromUTF8("ソーサラー&コンジャラー"), wxString::FromUTF8("技8    体5    心16"), wxString::FromUTF8("1000点")},
        {wxString::FromUTF8("吟遊詩人"), wxString::FromUTF8("バード"), wxString::FromUTF8("技7    体7    心15"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("練体士"), wxString::FromUTF8("エンハンサー"), wxString::FromUTF8("技9    体9    心11"), wxString::FromUTF8("2500点")}
    };
    // ティエンス
    speciesData[wxString::FromUTF8("ティエンス")] = {
        {wxString::FromUTF8("騎手"), wxString::FromUTF8("ライダー"), wxString::FromUTF8("技10    体11    心7"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("拳闘士"), wxString::FromUTF8("グラップラー"), wxString::FromUTF8("技9    体13    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("戦士"), wxString::FromUTF8("ファイター"), wxString::FromUTF8("技8    体12    心8"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技7    体12    心9"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔法使い"), wxString::FromUTF8("ソーサラー"), wxString::FromUTF8("技6    体12    心10"), wxString::FromUTF8("2000点")},

        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技11    体12    心5"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("密偵"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技10    体10    心8"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("魔法戦士"), wxString::FromUTF8("ファイター&ソーサラー"), wxString::FromUTF8("技9    体11    心8"), wxString::FromUTF8("1000点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技7    体11    心10"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("学者"), wxString::FromUTF8("セージ"), wxString::FromUTF8("技8    体11    心9"), wxString::FromUTF8("2500点")}
    };
    // レプラカーン
    speciesData[wxString::FromUTF8("レプラカーン")] = {
        {wxString::FromUTF8("軽戦士"), wxString::FromUTF8("フェンサー"), wxString::FromUTF8("技13    体5    心5"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("射手"), wxString::FromUTF8("シューター"), wxString::FromUTF8("技12    体6    心5"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("密偵"), wxString::FromUTF8("スカウト"), wxString::FromUTF8("技14    体4    心5"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("妖精使い"), wxString::FromUTF8("フェアリーテイマー"), wxString::FromUTF8("技11    体4    心8"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("錬金術師"), wxString::FromUTF8("アルケミスト"), wxString::FromUTF8("技11    体5    心7"), wxString::FromUTF8("2500点")},

        {wxString::FromUTF8("野伏"), wxString::FromUTF8("レンジャー"), wxString::FromUTF8("技12    体5    心6"), wxString::FromUTF8("2500点")},
        {wxString::FromUTF8("趣味人"), wxString::FromUTF8("セージ&バード"), wxString::FromUTF8("技13    体4    心6"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔動機師"), wxString::FromUTF8("マギテック"), wxString::FromUTF8("技12    体4    心7"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("神官"), wxString::FromUTF8("プリースト"), wxString::FromUTF8("技10    体5    心8"), wxString::FromUTF8("2000点")},
        {wxString::FromUTF8("魔導師"), wxString::FromUTF8("ソーサラーorコンジャラー"), wxString::FromUTF8("技11    体3    心9"), wxString::FromUTF8("2000点")}
    };  
}



void Tab2Panel::OnSpeciesSelected(wxCommandEvent& event) {
    int selection = speciesListBox->GetSelection();
    if (selection == wxNOT_FOUND || selection == 0) return;

    wxString selectedSpecies = speciesListBox->GetString(selection);
    //wxLogMessage(wxString::FromUTF8("選択された種族: %s"), selectedSpecies);

    //wxLogMessage(wxString::FromUTF8("ここまで来た"));
    //wxLogDebug(wxString::FromUTF8("選択された種族: %s"), selectedSpecies);


    for (auto& [name, grid] : speciesTables) {
        if (name == selectedSpecies) {
            grid->Show();
            grid->SetMinSize(wxSize(500, 300)); // サイズ指定
            //wxLogMessage(wxString::FromUTF8("表示対象: %s"), name);
        }
        else {
            grid->Hide();
        }
    }

    // グリッドの親（通常は gridPanel）にもレイアウトを指示
    if (!speciesTables.empty()) {
        auto anyGrid = speciesTables.begin()->second;
        if (anyGrid) {
            anyGrid->GetParent()->Layout();
        }
    }

    Layout();  // Tab2Panel自体のレイアウトも更新
    Refresh();
}






void Tab2Panel::OnCreateButtonClicked(wxCommandEvent&) {
        // 名前の入力チェック
        wxString name = nameTextBox->GetValue();
        if (name.IsEmpty()) {
            nameErrorText->SetLabel(wxString::FromUTF8("このフィールドを入力してください"));
            nameErrorText->Show();
            this->Layout();
            return;
        }
        nameErrorText->Hide();
        
        // 種族の選択チェック
        int speciesIndex = speciesListBox->GetSelection();
        if (speciesIndex == 0) {
            wxMessageBox(wxString::FromUTF8("種族を選択してください"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            return;
        }
        // 種族の文字の取得
        wxString speciesName = speciesListBox->GetString(speciesIndex);
        
        // 試行結果を生成する処理

        trialResults.clear(); // 以前の試行を削除

        // 3回の試行を実行
        for (int i = 0; i < 3; ++i) {
            TrialResult result;

            result.name = std::string(name.mb_str());
            result.species = std::string(speciesName.mb_str());

            // 技、体、心の値を取得
            result.born[0] = skillSpinCtrl->GetValue();
            result.born[1] = bodySpinCtrl->GetValue();
            result.born[2] = mindSpinCtrl->GetValue();

            // 種族に応じたA~Fを計算
            CalculateAttributes(speciesIndex, result.dicenumber, result.diceeffect, result.ability, result.impurity, result.feature);
            std::cout << wxString::FromUTF8("After CalculateAttributes: result.dicenumber = ") << result.dicenumber << std::endl;

            // 各能力値とボーナスを計算
            CalculateAbilityValues(result);

            trialResults.push_back(result);
        }
        // 結果を表示
        DisplayResults();
    }
    

void Tab2Panel::CalculateAttributes(int speciesIndex, int& dicenumber, int& diceeffect, int* ability,int& impurity, std::string& feature) {
    // 種族に応じてA~Fを計算 (リストボックスのインデックスと対応)
    switch (speciesIndex) {
    case 1: // 人間
        dicenumber = 12;
        diceeffect = 0;
        feature = wxString::FromUTF8("[剣の加護/運命変転]");
        impurity = 0;
        for (int i = 0; i < 6; ++i) ability[i] = RollDice(2, 0);
        break;
    case 2: // エルフ
        dicenumber = 11;
        diceeffect = 0;
        impurity = 0;
        feature = wxString::FromUTF8("[暗視][剣の加護/優しき水]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0); // A,B
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 0); // C,D
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0); // E,F   
        break;
    case 3: // ドワーフ
        dicenumber = 10;
        diceeffect = 1.2;
        impurity = 0;
        feature = wxString::FromUTF8("[暗視][剣の加護/炎身]");
        ability[0] = RollDice(2, 6); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 6);
        break;
    case 4: // タビット
        dicenumber = 9;
        diceeffect = 0.6667;
        impurity = 0;
        feature = wxString::FromUTF8("[第六感]");
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 6); ability[5] = RollDice(2, 0);
        break;
    case 5: // ルーンフォーク
        dicenumber = 10;
        diceeffect = 0;
        impurity = 0;
        feature =  wxString::FromUTF8("[暗視][HP変換]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(1, 0);
        break;
    case 6: // ナイトメア
        dicenumber = 10;
        diceeffect = 0;
        impurity = 1;
        feature =  wxString::FromUTF8("[異貌][弱点(属性)]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(1, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0);
        break;
    case 7: // リカント
        dicenumber = 8;
        diceeffect = 1.125;
        impurity = 0;
        feature =  wxString::FromUTF8("[暗視(獣変貌)][獣変貌]");
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 3);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(1, 6); ability[5] = RollDice(1, 0);
        break;
    case 8: // リルドラケン
        dicenumber = 10;
        diceeffect = 0.6;
        impurity = 0;
        feature =  wxString::FromUTF8("[鱗の皮膚][尻尾が武器][剣の加護/風の翼]");
        ability[0] = RollDice(1, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 0);
        break;
    case 9: // グラスランナー
        dicenumber = 10;
        diceeffect = 1.2;
        impurity = 0;
        feature =  wxString::FromUTF8("[マナ不干渉][虫や植物との意思疎通]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(2, 6);
        break;
    case 10: // メリア
        dicenumber = 7;
        diceeffect = 0.8571;
        impurity = 0;
        feature =  wxString::FromUTF8("[繁茂する生命]");
        ability[0] = RollDice(1, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(2, 6);
        ability[4] = RollDice(1, 0); ability[5] = RollDice(1, 0);
        break;
    case 11: // ティエンス
        dicenumber = 10;
        diceeffect = 0.6;
        impurity = 0;
        feature = wxString::FromUTF8("[通じ合う意識]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(2, 0);
        ability[2] = RollDice(1, 0); ability[3] = RollDice(1, 3);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 3);
        break;
    case 12: // レプラカーン
        dicenumber = 11;
        diceeffect = 0;
        impurity = 0;
        feature = wxString::FromUTF8("[暗視][見えざる手][姿なき職人]");
        ability[0] = RollDice(2, 0); ability[1] = RollDice(1, 0);
        ability[2] = RollDice(2, 0); ability[3] = RollDice(2, 0);
        ability[4] = RollDice(2, 0); ability[5] = RollDice(2, 0);
        break;
    default:
        std::cerr << wxString::FromUTF8("エラー: 無効な種族インデックス") << std::endl;
        dicenumber = 0;
        diceeffect = 0;
    }
    std::cout << wxString::FromUTF8("CalculateAttributes: dicenumber = ") << dicenumber << std::endl;
 }

void Tab2Panel::CalculateAbilityValues(TrialResult& result) {
    // 技, 体, 心 + A~F に基づき能力値を計算
    result.attributes[0] = result.born[0] + result.ability[0]; // 器用度 = 技 + A
    result.attributes[1] = result.born[0] + result.ability[1]; // 敏捷度 = 技 + B
    result.attributes[2] = result.born[1] + result.ability[2]; // 筋力 = 体 + C
    result.attributes[3] = result.born[1] + result.ability[3]; // 生命力 = 体 + D
    result.attributes[4] = result.born[2] + result.ability[4]; // 知力 = 心 + E
    result.attributes[5] = result.born[2] + result.ability[5]; // 精神力 = 心 + F

    result.pretotal = 0;
    result.total = 0;
    for (int i = 0; i < 6; ++i) {
        result.bonuses[i] = result.attributes[i] / 6;
        result.pretotal += result.ability[i];
        result.total += result.attributes[i];
        std::cout << wxString::FromUTF8("result.total (for loop中): ") << result.total << std::endl;
    }

    // int除算の性質を応用して小数第五位を切り上げ
    int average = (result.pretotal*10000) / result.dicenumber;
    // もう一度10^4で割ることで結果を正す
    float average2 = average / 10000.0f;
    result.average = average2 - result.diceeffect;
}



void Tab2Panel::DisplayResults() {
    // ダイアログタイトルの作成
    wxString dialogTitle;
    if (!trialResults.empty()) {
        dialogTitle = wxString::Format(wxString::FromUTF8("試行結果: %s"), trialResults[0].name);
    }
    else {
        dialogTitle = wxString::FromUTF8("試行結果");
    }

    // ダイアログウィンドウを作成
    wxDialog* resultDialog = new wxDialog(this, wxID_ANY, dialogTitle, wxDefaultPosition, wxSize(506, 450));
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);

    // 3回の試行結果を表示
    for (size_t i = 0; i < trialResults.size(); ++i) {
        const TrialResult& result = trialResults[i];

        // 試行ヘッダーとボタンのレイアウト
        wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

        // 試行ヘッダーを作成
        wxStaticText* header = new wxStaticText(resultDialog, wxID_ANY,
            wxString::Format(wxString::FromUTF8("試行%d: %s"), (int)(i + 1), result.species));
        headerSizer->Add(header, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

        // 「この能力値で新規作成」ボタン
        wxButton* createButton = new wxButton(resultDialog, wxID_ANY, wxString::FromUTF8("この能力値で新規作成"));
        createButton->Bind(wxEVT_BUTTON, [this, result, resultDialog](wxCommandEvent&) {
            // tab1Panel が正しいか確認
            if (tab1Panel) {
                std::vector<int> characterData = {
                    result.born[0], result.born[1], result.born[2],  // 技・体・心
                    result.ability[0], result.ability[1], result.ability[2], result.ability[3], result.ability[4], result.ability[5], // A～F
                    result.impurity
                };
                                
                tab1Panel->SetCharacterData(result.name, result.species, result.feature, characterData);
            }
            else {
                wxMessageBox(wxString::FromUTF8("Tab1Panelが見つかりません。"), wxString::FromUTF8("エラー"), wxOK | wxICON_ERROR);
            }

            // ダイアログを閉じる前にタブを切り替えない
            resultDialog->EndModal(wxID_OK);  // ダイアログを閉じる
            });
        headerSizer->Add(createButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

        // 試行ヘッダーをダイアログに追加
        dialogSizer->Add(headerSizer, 0, wxEXPAND | wxALL, 5);

        // グリッドを作成
        wxGrid* grid = new wxGrid(resultDialog, wxID_ANY);
        grid->CreateGrid(4, 6); // 行数と列数を変更

        // 列と行のラベルを非表示
        grid->SetColLabelSize(0);
        grid->SetRowLabelSize(0);

        // セルの結合設定
        grid->SetCellSize(0, 0, 1, 2);
        grid->SetCellSize(0, 2, 1, 2);
        grid->SetCellSize(0, 4, 1, 2);
        grid->SetCellSize(3, 0, 1, 6);

        // セルの中央寄せ
        int rows = grid->GetNumberRows();
        int cols = grid->GetNumberCols();
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                grid->SetCellAlignment(row, col, wxALIGN_CENTER, wxALIGN_CENTER);
            }
        }

        // 結果をグリッドにセット
        grid->SetCellValue(0, 0, wxString::Format(wxString::FromUTF8("技: %d"), result.born[0]));
        grid->SetCellValue(0, 2, wxString::Format(wxString::FromUTF8("体: %d"), result.born[1]));
        grid->SetCellValue(0, 4, wxString::Format(wxString::FromUTF8("心: %d"), result.born[2]));

        grid->SetCellValue(1, 0, wxString::Format(wxString::FromUTF8("A: %d"), result.ability[0]));
        grid->SetCellValue(1, 1, wxString::Format(wxString::FromUTF8("B: %d"), result.ability[1]));
        grid->SetCellValue(1, 2, wxString::Format(wxString::FromUTF8("C: %d"), result.ability[2]));
        grid->SetCellValue(1, 3, wxString::Format(wxString::FromUTF8("D: %d"), result.ability[3]));
        grid->SetCellValue(1, 4, wxString::Format(wxString::FromUTF8("E: %d"), result.ability[4]));
        grid->SetCellValue(1, 5, wxString::Format(wxString::FromUTF8("F: %d"), result.ability[5]));

        grid->SetCellValue(2, 0, wxString::Format(wxString::FromUTF8("器用: %d"), result.attributes[0]));
        grid->SetCellValue(2, 1, wxString::Format(wxString::FromUTF8("敏捷: %d"), result.attributes[1]));
        grid->SetCellValue(2, 2, wxString::Format(wxString::FromUTF8("筋力: %d"), result.attributes[2]));
        grid->SetCellValue(2, 3, wxString::Format(wxString::FromUTF8("生命: %d"), result.attributes[3]));
        grid->SetCellValue(2, 4, wxString::Format(wxString::FromUTF8("知力: %d"), result.attributes[4]));
        grid->SetCellValue(2, 5, wxString::Format(wxString::FromUTF8("精神: %d"), result.attributes[5]));

        grid->SetCellValue(3, 0, wxString::Format(wxString::FromUTF8("ダイス平均: %.4f / 能力値合計: %d"), result.average, result.total));

        grid->EnableEditing(false); // 編集不可に設定
        // グリッドをダイアログに追加
        dialogSizer->Add(grid, 0, wxEXPAND | wxALL, 5);
    }

    // 閉じるボタン
    wxButton* closeButton = new wxButton(resultDialog, wxID_OK, wxString::FromUTF8("閉じる"));
    dialogSizer->Add(closeButton, 0, wxALIGN_CENTER | wxALL, 10);

    resultDialog->SetSizer(dialogSizer);
    resultDialog->ShowModal();

    // ダイアログが閉じた後にタブを切り替え
    if (notebook) {
        notebook->SetSelection(0);  // Tab1Panel に切り替え
    }

    resultDialog->Destroy();  // ダイアログを閉じる際にメモリを解放
}
