#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filesystem>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

struct Meritev
{
    std::filesystem::path meritev;
    std::filesystem::path leto;
    std::filesystem::path jama;

    bool Valid()
    {
        return (!meritev.empty() && !leto.empty() && !jama.empty());
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_vnesiButton_clicked();

    void on_novaMeritevFile_clicked();
    void on_novaMeritevFolder_clicked();
    void on_novaLetoFile_clicked();
    void on_novaLetoFolder_clicked();
    void on_novaJamaFile_clicked();
    void on_novaJamaFolder_clicked();

    void on_staraMeritevFile_clicked();
    void on_staraMeritevFolder_clicked();
    void on_staraLetoFile_clicked();
    void on_staraLetoFolder_clicked();
    void on_staraJamaFile_clicked();
    void on_staraJamaFolder_clicked();

    void on_novaMeritevEdit_clicked();

    void on_novaLetoEdit_clicked();

    void on_novaJamaEdit_clicked();

    void on_staraMeritevEdit_clicked();

    void on_staraLetoEdit_clicked();

    void on_staraJamaEdit_clicked();

private:
    Ui::MainWindow *ui;
    std::filesystem::path get_svx_file(std::filesystem::path open_path = "");
    std::filesystem::path canonical_path(std::filesystem::path path);
    std::filesystem::path get_svx_file_in_folder(std::filesystem::path path);
    std::vector<std::string> read_file(std::filesystem::path path);
    void write_file(std::filesystem::path path, std::vector<std::string> &lines);
    void add_measurment();
    void autocomplete(Meritev &meritev);

    std::filesystem::path m_DefaultPath = "/home/luka/dev/qt/kanin_rombon/surveys/brezno_specega_dinozavra/2021";

    Meritev m_Nova{};
    Meritev m_Stara{};
};
#endif // MAINWINDOW_H
