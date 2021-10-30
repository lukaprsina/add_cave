#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "platform.h"

#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::vector<std::string> config = read_file("config.txt", true);
    if (config.size() == 1)
        m_DefaultPath = config[0];
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_novaMeritevFolder_clicked()
{
    std::filesystem::path folder = get_svx_file(m_DefaultPath);
    if (folder.empty())
        return;

    m_Nova.meritev = folder;
    ui->novaMeritevPath->setText(QString::fromStdString(m_Nova.meritev.generic_string()));

    auto leto_folder = canonical_path(m_Nova.meritev.parent_path().parent_path());
    m_Nova.leto = canonical_path(get_svx_file_in_folder(leto_folder));
    ui->novaLetoPath->setText(QString::fromStdString(m_Nova.leto.generic_string()));

    auto jama_folder = canonical_path(m_Nova.leto.parent_path().parent_path());
    m_Nova.jama = canonical_path(get_svx_file_in_folder(jama_folder));
    ui->novaJamaPath->setText(QString::fromStdString(m_Nova.jama.generic_string()));
    m_DefaultPath = jama_folder;
}

void MainWindow::on_staraMeritevFolder_clicked()
{
    std::filesystem::path folder = get_svx_file(m_DefaultPath);
    if (folder.empty())
        return;

    m_Stara.meritev = folder;
    ui->staraMeritevPath->setText(QString::fromStdString(m_Stara.meritev.generic_string()));

    auto leto_folder = canonical_path(m_Stara.meritev.parent_path().parent_path());
    m_Stara.leto = canonical_path(get_svx_file_in_folder(leto_folder));
    ui->staraLetoPath->setText(QString::fromStdString(m_Stara.leto.generic_string()));

    auto jama_folder = canonical_path(m_Stara.leto.parent_path().parent_path());
    m_Stara.jama = canonical_path(get_svx_file_in_folder(jama_folder));
    ui->staraJamaPath->setText(QString::fromStdString(m_Stara.jama.generic_string()));
    m_DefaultPath = jama_folder;
}

void MainWindow::on_novaLetoFolder_clicked()
{
    m_Nova.leto = get_svx_file(m_DefaultPath);
    ui->novaLetoPath->setText(QString::fromStdString(m_Nova.leto.generic_string()));
}

void MainWindow::on_staraLetoFolder_clicked()
{
    m_Stara.leto = get_svx_file(m_DefaultPath);
    ui->staraLetoPath->setText(QString::fromStdString(m_Stara.leto.generic_string()));
}

void MainWindow::on_novaJamaFolder_clicked()
{
    m_Nova.jama = get_svx_file(m_DefaultPath);
    ui->novaJamaPath->setText(QString::fromStdString(m_Nova.jama.generic_string()));
}

void MainWindow::on_staraJamaFolder_clicked()
{
    m_Stara.jama = get_svx_file(m_DefaultPath);
    ui->staraJamaPath->setText(QString::fromStdString(m_Stara.jama.generic_string()));
}

void MainWindow::on_vnesiButton_clicked()
{
    m_Nova.meritev = canonical_path(std::filesystem::path(ui->novaMeritevPath->text().toStdString()));
    m_Nova.leto = canonical_path(std::filesystem::path(ui->novaLetoPath->text().toStdString()));
    m_Nova.jama = canonical_path(std::filesystem::path(ui->novaJamaPath->text().toStdString()));

    if (!m_Nova.Valid() || !m_Stara.Valid() ||
        ui->novaTocka->text().isEmpty() ||
        ui->staraTocka->text().isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Dodaj jamo"));
        msgBox.setText(QString::fromStdString("Izpolnite vsa polja."));
        msgBox.exec();
        return;
    }

    {
        auto lines = read_file(m_Nova.leto);
        std::vector<std::vector<std::string>::iterator> end_statements;
        for (auto it = lines.begin(); it < lines.end(); it++)
        {
            if (it->find("*end ") != std::string::npos)
                end_statements.emplace_back(it);
        }

        std::string folder = m_Nova.meritev.parent_path().filename().generic_string();
        std::string name = m_Nova.meritev.stem().generic_string();

        std::string include = "*include " + folder + "/" + name;

        if (end_statements.size() == 1)
            lines.insert(end_statements[0] - 1, include);

        write_file(m_Nova.leto, lines);
    }

    {
        auto lines = read_file(m_Nova.jama);
        std::vector<std::vector<std::string>::iterator> end_statements;
        for (auto it = lines.begin(); it < lines.end(); it++)
        {
            if (it->find("*end ") != std::string::npos)
                end_statements.emplace_back(it);
        }

        std::string new_year = m_Nova.leto.parent_path().filename().generic_string();
        std::string new_name = m_Nova.meritev.stem().generic_string();

        std::string old_year = m_Stara.leto.parent_path().filename().generic_string();
        std::string old_name = m_Stara.meritev.stem().generic_string();

        std::string include = "*equate " + new_year + "." + new_name + "." +
                              ui->novaTocka->text().toStdString() + " " +
                              old_year + "." + old_name + "." +
                              ui->staraTocka->text().toStdString();

        if (end_statements.size() == 1)
            lines.insert(end_statements[0] - 1, include);

        write_file(m_Nova.jama, lines);
    }

    std::vector<std::string> config = {m_Nova.jama.parent_path().parent_path().generic_string()};
    write_file("config.txt", config);
    m_DefaultPath = std::filesystem::path(config[0]);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Dodaj jamo"));
    msgBox.setText(QString::fromStdString("Meritve so vnešene."));
    msgBox.exec();
}

std::vector<std::string> MainWindow::read_file(std::filesystem::path path, bool create)
{
    std::ifstream in(path.c_str(),
                     std::ios_base::in |
                         (create ? (std::ios_base::app | std::ios_base::out) : std::ios_base::app));
    if (!in && !create)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Dodaj jamo"));
        msgBox.setText(QString::fromStdString("Datoteke ni mogoče prebrati."));
        msgBox.exec();
        return {};
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(in, line))
        lines.emplace_back(line);

    return lines;
}

void MainWindow::write_file(std::filesystem::path path, std::vector<std::string> &lines)
{
    std::ofstream out(path.c_str(), std::ios_base::out);
    if (!out)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Dodaj jamo"));
        msgBox.setText(QString::fromStdString("Datoteke ni mogoče prebrati."));
        msgBox.exec();
        return;
    }

    for (auto &string : lines)
        out << string << std::endl;
}

std::filesystem::path MainWindow::get_svx_file(std::filesystem::path open_path)
{
    QString name = QFileDialog::getOpenFileName(this, tr("Open survex file"), QString::fromStdString(canonical_path(open_path).generic_string()), tr("svx Files (*.svx)"));
    auto path_name = canonical_path(name.toStdString());
    return path_name;
}

std::filesystem::path MainWindow::canonical_path(std::filesystem::path path)
{
    std::filesystem::path svx_path;
    std::error_code error_code;
    svx_path = std::filesystem::canonical(path, error_code);

    if (path == "")
        error_code.clear();

    if (error_code.value() != 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Dodaj jamo"));
        msgBox.setText(QString::fromStdString(
            std::string(std::string("The path ") + path.generic_string() + std::string(" isn't a real path.")).c_str()));
        msgBox.exec();
        svx_path.clear();
    }

    return svx_path;
}

std::filesystem::path MainWindow::get_svx_file_in_folder(std::filesystem::path path)
{
    int number_of_svx_files = 0;
    std::filesystem::path svx_file;
    for (auto &p : std::filesystem::directory_iterator(path))
    {
        if (p.path().extension() == ".svx")
        {
            number_of_svx_files++;
            svx_file = p.path();
        }
    }

    if (number_of_svx_files != 1)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Dodaj jamo"));

        if (number_of_svx_files == 2)
            msgBox.setText(QString::fromStdString(
                std::string(std::string("V mapi ") + path.generic_string() + std::string(" sta ") + std::to_string(number_of_svx_files) + std::string(" .svx datoteki.")).c_str()));
        else
            msgBox.setText(QString::fromStdString(
                std::string(std::string("V mapi ") + path.generic_string() + std::string(" je ") + std::to_string(number_of_svx_files) + std::string(" .svx datotek.")).c_str()));

        msgBox.setInformativeText("Katero hočete izbrati?");
        msgBox.setStandardButtons(QMessageBox::Open);
        msgBox.setDefaultButton(QMessageBox::Open);
        msgBox.exec();
        svx_file = get_svx_file(path);
    }

    return svx_file;
}

void MainWindow::open_folder(std::filesystem::path &path)
{
    std::filesystem::path folder = path.parent_path();
    open_file(folder);
}

void MainWindow::on_novaMeritevFile_clicked()
{
    open_file(m_Nova.meritev);
}

void MainWindow::on_novaLetoFile_clicked()
{
    open_file(m_Nova.leto);
}

void MainWindow::on_novaJamaFile_clicked()
{
    open_file(m_Nova.jama);
}

void MainWindow::on_staraMeritevFile_clicked()
{
    open_file(m_Stara.meritev);
}

void MainWindow::on_staraLetoFile_clicked()
{
    open_file(m_Stara.leto);
}

void MainWindow::on_staraJamaFile_clicked()
{
    open_file(m_Stara.jama);
}

void MainWindow::on_novaMeritevEdit_clicked()
{
    open_folder(m_Nova.meritev);
}

void MainWindow::on_novaLetoEdit_clicked()
{
    open_folder(m_Nova.leto);
}

void MainWindow::on_novaJamaEdit_clicked()
{
    open_folder(m_Nova.jama);
}

void MainWindow::on_staraMeritevEdit_clicked()
{
    open_folder(m_Stara.meritev);
}

void MainWindow::on_staraLetoEdit_clicked()
{
    open_folder(m_Stara.leto);
}

void MainWindow::on_staraJamaEdit_clicked()
{
    open_folder(m_Stara.jama);
}
