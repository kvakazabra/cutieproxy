#include "CpFiltersDialog.h"
#include "ui_CpFiltersDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

#include <Windows.h>

CpFiltersDialog::CpFiltersDialog(QWidget* parent, QProxyItem* proxy)
	: m_Ui{ new Ui::FiltersDialog() }, m_Proxy{ proxy } {
	m_Ui->setupUi(this);

	init();
	connectSignals();
}

CpFiltersDialog::~CpFiltersDialog() {
	delete m_Ui;
	m_Ui = nullptr;
}

std::shared_ptr<iphelper::filter_interface> CpFiltersDialog::filter() const {
	TFilterType type = currentFilterType();

	switch (type) {
	case TFilterType::None: {
		return std::make_shared<NoneFilter>();
	}
	case TFilterType::All: {
		return std::make_shared<AllFilter>();
	}
	case TFilterType::Name: {
		std::wstring str = m_Ui->patternLineEdit->text().toStdWString();
		bool isOk{ };
		TNameFilterType nameFilterType =
			static_cast<TNameFilterType>(
				m_Ui->nameFilterTypeCombo->itemData(m_Ui->nameFilterTypeCombo->currentIndex(), Qt::UserRole).toInt(&isOk)
				);
		if (!isOk) {
			return { };
		}

		TStringMatchType stringMatchType =
			static_cast<TStringMatchType>(
				m_Ui->stringMatchTypeCombo->itemData(m_Ui->stringMatchTypeCombo->currentIndex(), Qt::UserRole).toInt(&isOk)
				);
		if (!isOk) {
			return { };
		}

		bool caseSensitive = m_Ui->caseSensitiveCheckBox->isChecked();

		return std::make_shared<NameFilter>(nameFilterType, str, stringMatchType, caseSensitive);
	}
	case TFilterType::ProcessId: {
		return std::make_shared<ProcessIdFilter>(convertStringToIds(m_Ui->idsLineEdit->text()));
	}
	}

	return { };
}

void CpFiltersDialog::init() {
	m_Ui->filterTypeCombo->addItem(QObject::tr("None"), QVariant(static_cast<int>(TFilterType::None)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("All"), QVariant(static_cast<int>(TFilterType::All)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("Name"), QVariant(static_cast<int>(TFilterType::Name)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("Process Id"), QVariant(static_cast<int>(TFilterType::ProcessId)));

	m_Ui->nameFilterTypeCombo->addItem(QObject::tr("Process Name"), QVariant(static_cast<int>(TNameFilterType::ProcessName)));
	m_Ui->nameFilterTypeCombo->addItem(QObject::tr("Image Path"), QVariant(static_cast<int>(TNameFilterType::ImagePath)));

	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Exact"), QVariant(static_cast<int>(TStringMatchType::Exact)));
	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Contains"), QVariant(static_cast<int>(TStringMatchType::Contains)));
	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Regular Expression"), QVariant(static_cast<int>(TStringMatchType::RegExp)));

	m_FilterTypeToLayout[TFilterType::Name] = m_Ui->nameFilterGroupBox;
	m_FilterTypeToLayout[TFilterType::ProcessId] = m_Ui->processIdGroupBox;

	for (auto& filter : m_Proxy->filters()) {
		m_Ui->filterTypeCombo->setCurrentIndex(
			static_cast<int>(filterTypeByObject(filter))
		);
		filterTypeChanged();
	}
}

std::set<ProcessIdFilter::TProcessId> CpFiltersDialog::convertStringToIds(const QString& input) {
	if (input.isEmpty()) {
		return { };
	}

	std::set<ProcessIdFilter::TProcessId> result{ };
	QStringList parts = input.split(';', Qt::SkipEmptyParts);

	for (auto& part : parts) {
		bool isOk{ };
		static_assert(sizeof(ProcessIdFilter::TProcessId) == 4, "rework this fn");
		int v = part.toUInt(&isOk);
		if (!isOk) {
			continue;
		}

		result.insert(v);
	}

	return result;
}

CpFiltersDialog::TFilterType CpFiltersDialog::filterTypeByObject(const std::shared_ptr<iphelper::filter_interface>& filter) {
	auto ptr = filter.get();
	if (dynamic_cast<AllFilter*>(ptr)) {
		return TFilterType::All;
	}

	if (dynamic_cast<NoneFilter*>(ptr)) {
		return TFilterType::None;
	}

	if (dynamic_cast<NameFilter*>(ptr)) {
		return TFilterType::Name;
	}

	if (dynamic_cast<ProcessIdFilter*>(ptr)) {
		return TFilterType::ProcessId;
	}

	return TFilterType::None; // an actual filter type
}

CpFiltersDialog::TFilterType CpFiltersDialog::currentFilterType() const {
	bool isOk{ };
	int itype = m_Ui->filterTypeCombo->itemData(m_Ui->filterTypeCombo->currentIndex(), Qt::UserRole).toInt(&isOk);
	if (!isOk) {
		throw std::runtime_error("smth is wrong");
	}

	TFilterType type = static_cast<TFilterType>(itype);
	switch (type) {
	case TFilterType::None:
	case TFilterType::All:
	case TFilterType::Name:
	case TFilterType::ProcessId:
		break;
	default: throw std::runtime_error("no such filter type");
	}

	return type;
}

void CpFiltersDialog::filterTypeChanged() {
	TFilterType type = currentFilterType();

	for (auto& [filterType, groupBox] : m_FilterTypeToLayout) {
		if (!groupBox) {
			continue;
		}

		groupBox->setHidden(true);
	}
 
	if (QGroupBox* groupBox = m_FilterTypeToLayout[type]
		; groupBox) {
		groupBox->setVisible(true);
	}

	adjustSize();
}

void CpFiltersDialog::addProcessId(ProcessIdFilter::TProcessId id) {
	setWindowTitle(m_OldWindowTitle);
	setEnabled(true);

	m_Ui->idsLineEdit->setText(QString("%1;%2").arg(id).arg(m_Ui->idsLineEdit->text()));
}

struct TMouseHookProcData {
	HHOOK mouseHk{ };
	std::set<DWORD> ignoreProcessIds{ };
	CpFiltersDialog* dialog{ };
};

static std::unique_ptr<TMouseHookProcData> s_MouseHookProcData{ };

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(s_MouseHookProcData->mouseHk, nCode, wParam, lParam);
	}

	if (wParam != WM_LBUTTONDOWN) {
		return CallNextHookEx(s_MouseHookProcData->mouseHk, nCode, wParam, lParam);
	}

	auto* mouseInfo = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
	POINT pt = { mouseInfo->pt.x, mouseInfo->pt.y };
	HWND clickedWindow = WindowFromPoint(pt);
	if (!clickedWindow) {
		return CallNextHookEx(s_MouseHookProcData->mouseHk, nCode, wParam, lParam);
	}

	DWORD processId = { };
	GetWindowThreadProcessId(clickedWindow, &processId);

	if (!processId || s_MouseHookProcData->ignoreProcessIds.count(processId) != 0) {
		return CallNextHookEx(s_MouseHookProcData->mouseHk, nCode, wParam, lParam);
	}

	UnhookWindowsHookEx(s_MouseHookProcData->mouseHk);
	s_MouseHookProcData->mouseHk = nullptr;

	emit s_MouseHookProcData->dialog->addProcessIdSignal(processId);

	return CallNextHookEx(s_MouseHookProcData->mouseHk, nCode, wParam, lParam);
}

void CpFiltersDialog::connectSignals() {
	QObject::connect(m_Ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
	QObject::connect(m_Ui->applyChangesButton, &QPushButton::clicked, this, &QDialog::accept);

	QObject::connect(m_Ui->filterTypeCombo, &QComboBox::currentIndexChanged, this, &CpFiltersDialog::filterTypeChanged);
	QObject::connect(this, &CpFiltersDialog::addProcessIdSignal, this, &CpFiltersDialog::addProcessId);

	QObject::connect(m_Ui->selectByWindowButton, &QPushButton::clicked, [this]() -> void {
		m_OldWindowTitle = windowTitle();
		setEnabled(false);
		setWindowTitle(QObject::tr("Waiting for a window..."));

		s_MouseHookProcData = std::make_unique<TMouseHookProcData>();
		s_MouseHookProcData->ignoreProcessIds = convertStringToIds(m_Ui->idsLineEdit->text());
		s_MouseHookProcData->ignoreProcessIds.insert(GetCurrentProcessId());
		s_MouseHookProcData->dialog = this;
		s_MouseHookProcData->mouseHk = SetWindowsHookExA(WH_MOUSE_LL, MouseHookProc, 0, 0);
		});
}