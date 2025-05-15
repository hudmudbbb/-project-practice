#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// --- ДАННЫЕ О СЕРВИСАХ --- //
struct ServicePolicy {
    string name;
    int minLength;
    int maxLength;
    bool requireLower;
    bool requireUpper;
    bool requireDigit;
    bool requireSpecial;
    bool allowUnicode;
    bool checkCompromised;
    bool compositionRequired;
    bool allowHardware2FA;
    string notes;
    vector<string> recommendations;
};

vector<ServicePolicy> getPolicies() {
    vector<ServicePolicy> policies = {
        // Госуслуги
        {
            "Госуслуги",
            8,
            64,
            true,
            true,
            true,
            true,
            false,
            true,
            true,
            false,
            "Сервис требует сложных паролей, но NIST рекомендует фокусироваться на длине и уникальности.",
            {
                "Используйте фразы длиной не менее 8 символов.",
                "Избегайте повторения старых паролей.",
                "Рекомендуется использовать менеджер паролей.",
                "Желательно использовать аппаратные ключи 2FA, если появится поддержка."
            }
        },
        // Gmail
        {
            "Gmail",
            8,
            100,
            false,
            false,
            false,
            false,
            false,
            true,
            false,
            true,
            "Сервис ориентируется на NIST SP 800-63B, но ограничен латиницей.",
            {
                "Пароль должен содержать не менее 8 символов.",
                "Избегайте популярных/простых паролей (например, password, 12345678).",
                "Рекомендуется использовать двухфакторную аутентификацию с аппаратным ключом.",
                "Желательно использовать менеджер паролей."
            }
        },
        // Telegram
        {
            "Telegram",
            6,
            64,
            false,
            false,
            false,
            false,
            true,
            false,
            false,
            false,
            "Нет строгих требований к символам. Минимальная длина ниже стандартов NIST.",
            {
                "Используйте длинные, необычные фразы (от 8 символов).",
                "Рекомендуется отказаться от коротких паролей.",
                "Для максимальной защиты включите двухфакторную аутентификацию."
            }
        },
        // Сбербанк Онлайн
        {
            "Сбербанк Онлайн",
            8,
            30,
            true,
            true,
            true,
            false,
            false,
            true,
            true,
            false,
            "Максимум 30 символов — ниже рекомендаций NIST.",
            {
                "Старайтесь не использовать последовательности или личные данные в пароле.",
                "Регулярно обновляйте пароль и проверяйте его уникальность.",
                "Желательно сделать пароль длиннее 12 символов."
            }
        },
        // Яндекс Почта
        {
            "Яндекс Почта",
            8,
            64,
            true,
            true,
            false,
            false,
            false,
            false,
            false,
            false,
            "Нет поддержки аппаратных ключей и Unicode.",
            {
                "Пароль должен состоять из латинских букв разного регистра.",
                "Желательно добавить цифры и специальные символы.",
                "Пароль не должен совпадать с вашим логином.",
                "Старайтесь использовать пароль длиной не менее 12 символов."
            }
        },
        // Авито
        {
            "Авито",
            8,
            64, // максимальное ограничение не оговорено, используем рекомендацию NIST как де-факто
            false, // нет обязательных требований к составу
            false,
            false,
            false,
            false,
            false,
            false,
            false,

            "Можно использовать спецсимволы. Нет проверки пароля в утёкших базах и Unicode, 2FA только через SMS.",
            {
                "Минимум 8 символов — хорошая практика.",
                "Для повышения стойкости используйте длинную парольную фразу.",
                "Разрешены спецсимволы, используйте смешанные символы.",
                "Не применяйте простые заменители символов (например, 'a' на '@').",
                "Лучше не использовать пароли из других сервисов."
            }
        }
    };
    return policies;
}

// --- ФУНКЦИИ ПРОВЕРКИ --- //

bool hasLower(const string &s) {
    return regex_search(s, regex("[a-z]"));
}
bool hasUpper(const string &s) {
    return regex_search(s, regex("[A-Z]"));
}
bool hasDigit(const string &s) {
    return regex_search(s, regex("[0-9]"));
}
bool hasSpecial(const string &s) {
    return regex_search(s, regex("[^a-zA-Z0-9]"));
}
bool hasNonLatin(const string &s) {
    return regex_search(s, regex("[^a-zA-Z0-9\\p{Punct}]"));
}
bool isCommonPassword(const string& s) {
    vector<string> bad = {"password", "123456", "12345678", "qwerty", "111111", "abc123"};
    string pwd = s;
    transform(pwd.begin(), pwd.end(), pwd.begin(), ::tolower);
    return find(bad.begin(), bad.end(), pwd) != bad.end();
}
bool containsLogin(const string& password, const string& login) {
    if (login.empty()) return false;
    string low_pwd = password, low_login = login;
    transform(low_pwd.begin(), low_pwd.end(), low_pwd.begin(), ::tolower);
    transform(low_login.begin(), low_login.end(), low_login.begin(), ::tolower);
    return low_pwd.find(low_login) != string::npos;
}

// --- ОСНОВНАЯ ПРОГРАММА --- //
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    auto policies = getPolicies();
    cout << "Выберите сервис для проверки пароля:\n";
    for (size_t i = 0; i < policies.size(); ++i) {
        cout << i+1 << ". " << policies[i].name << endl;
    }
    int choice;
    cin >> choice; cin.ignore();
    if (choice < 1 || choice > (int)policies.size()) {
        cout << "Некорректный выбор!\n";
        return 1;
    }
    ServicePolicy policy = policies[choice-1];

    string login;
    if (policy.name == "Яндекс Почта") {
        cout << "Введите ваш логин (для проверки сходства): ";
        getline(cin, login);
    }

    cout << "Введите пароль: ";
    string pw;
    getline(cin, pw);

    // Проверки
    cout << endl << "Проверка пароля для \"" << policy.name << "\"\n";
    bool ok = true;

    // Длина пароля
    if ((int)pw.size() < policy.minLength) {
        cout << "- Пароль слишком короткий! Минимум " << policy.minLength << " символов.\n";
        ok = false;
    } else if ((int)pw.size() > policy.maxLength) {
        cout << "- Пароль слишком длинный для этого сервиса. Максимум " << policy.maxLength << " символов.\n";
        ok = false;
    } else {
        cout << "+ Длина пароля допустима.\n";
    }

    // Проверка состава, если требуется
    if (policy.compositionRequired || policy.requireLower || policy.requireUpper || policy.requireDigit || policy.requireSpecial) {
        if (policy.requireLower && !hasLower(pw)) {
            cout << "- Требуется хотя бы одна маленькая латинская буква.\n";
            ok = false;
        }
        if (policy.requireUpper && !hasUpper(pw)) {
            cout << "- Требуется хотя бы одна ПРОПИСНАЯ латинская буква.\n";
            ok = false;
        }
        if (policy.requireDigit && !hasDigit(pw)) {
            cout << "- Требуется хотя бы одна цифра.\n";
            ok = false;
        }
        if (policy.requireSpecial && !hasSpecial(pw)) {
            cout << "- Требуется хотя бы один спецсимвол.\n";
            ok = false;
        }
    } else {
        cout << "+ Требования к составу символов отсутствуют или не обязательны.\n";
    }

    if (!policy.allowUnicode && hasNonLatin(pw)) {
        cout << "- В пароле есть недопустимые символы. Используйте только латиницу, цифры и спец. символы.\n";
        ok = false;
    }

    if ((policy.name == "Gmail" || policy.name == "Госуслуги") && isCommonPassword(pw)) {

        cout << "- Пароль слишком распространённый, попробуйте что-то сложнее.\n";
        ok = false;
    }
    if (policy.name == "Яндекс Почта" && containsLogin(pw, login)) {
        cout << "- Пароль не должен содержать ваш логин.\n";
        ok = false;
    }
    // Авито: OWASP не рекомендует обходные замены типа "a" -> "@" как метод усиления пароля
        if (policy.name == "Авито") {
        // Проверка на очевидные заменители (a -> @, o -> 0, i -> 1 и т.п.)
        // По рекомендации OWASP такие замены не делают пароль сильнее
        vector<pair<char, char>> substitutions = {{'a','@'}, {'o','0'}, {'i','1'}, {'s','$'}, {'e','3'}};
        bool found_subst = false;
        for (const auto& sub : substitutions) {
            auto pos = pw.find(sub.second);
            if (pos != string::npos && pw.find(sub.first) != string::npos) {
                cout << "- Рекомендация: не используйте простые заменители вроде '" << sub.first << "' на '" << sub.second << "'.\n";
                found_subst = true;
                break;
            }
        }
        if (!found_subst)
            cout << "+ Пароль не содержит тривиальных замен символов (a->@, o->0 и т.д.).\n";
        // Проверка допуска спецсимволов уже сделана выше (hasSpecial)
    }

    // Проверка на уникальность и длину
    if (pw.size() >= 16)
        cout << "+ Отличная длина пароля!\n";
    else if (pw.size() >= 12)
        cout << "+ Хорошая длина пароля.\n";
    else if (pw.size() >= policy.minLength)
        cout << "+ Минимальные требования по длине соблюдены.\n";

    // Итоговый вывод
    if (ok)
        cout << "\n✅ Пароль соответствует требованиям данного сервиса.\n";
    else
        cout << "\n❌ Пароль НЕ соответствует требованиям сервиса!\n";

    // Описание политики безопасности сервиса
    cout << "\nℹ️  Особенности политики безопасности " << policy.name << ":\n";
    cout << policy.notes << "\n";
    cout << "\nРекомендации:\n";
    for (const auto& rec : policy.recommendations)
        cout << "- " << rec << endl;

    // MFA возможность
    if (policy.allowHardware2FA)
        cout << "\n🔒 Рекомендуется использовать аппаратные ключи 2FA\n";
    else if (policy.name == "Авито")
        cout << "\nАвито поддерживает двухфакторную аутентификацию только через SMS, что менее надёжно по сравнению с аппаратными ключами или TOTP.\n";

    return 0;
}

