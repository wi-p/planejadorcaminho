#include "planejador.h"
#include <stdexcept>
#include <cmath>     /* sin, cos, etc */
#include <cctype>    /* isspace */
#include <fstream>
#include <string>
#include <algorithm> // find(), sort(), etc
#include <utility>
#include <vector>
/* ACRESCENTE SE NECESSARIO */


using namespace std;

/* *************************
   * CLASSE IDPONTO        *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDPonto::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE IDROTA         *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDRota::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE PONTO          *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Ponto& P)
{
  X << P.id << '\t' << P.nome << " (" <<P.latitude << ',' << P.longitude << ')';
  return X;
}

/// Distancia entre 2 pontos (formula de haversine)
/// NAO DEVE SER MODIFICADA
double Ponto::distancia(const Ponto& P) const
{
  // Gera excecao se pontos invalidos
  if (!valid() || !P.valid())
    throw invalid_argument("distancia: ponto(s) invalido(s)");

  // Tratar logo pontos identicos
  if (id == P.id) return 0.0;
  // Constantes
  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;
  // Conversao para radianos
  double lat1 = MY_PI*this->latitude/180.0;
  double lat2 = MY_PI*P.latitude/180.0;
  double lon1 = MY_PI*this->longitude/180.0;
  double lon2 = MY_PI*P.longitude/180.0;
  // Seno das diferencas
  double sin_dlat2 = sin((lat2-lat1)/2.0);
  double sin_dlon2 = sin((lon2-lon1)/2.0);
  // Quadrado do seno do angulo entre os pontos
  double sin2_ang = sin_dlat2*sin_dlat2 + cos(lat1)*cos(lat2)*sin_dlon2*sin_dlon2;
  // Em vez de utilizar a funcao arcosseno, asin(sqrt(sin2_ang)),
  // vou utilizar a funcao arcotangente, menos sensivel a erros numericos.
  // Distancia entre os pontos
  return 2.0*R_EARTH*atan2(sqrt(sin2_ang),sqrt(1-sin2_ang));
}

/* *************************
   * CLASSE ROTA           *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Rota& R)
{
  X << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
    << " [" << R.extremidade[0] << ',' << R.extremidade[1] << ']';
  return X;
}

/// Retorna a outra extremidade da rota, a que nao eh o parametro.
/// Gera excecao se o parametro nao for uma das extremidades da rota.
/// NAO DEVE SER MODIFICADA
IDPonto Rota::outraExtremidade(const IDPonto& ID) const
{
  if (extremidade[0]==ID) return extremidade[1];
  if (extremidade[1]==ID) return extremidade[0];
  throw invalid_argument("outraExtremidade: invalid IDPonto parameter");
}

/* *************************
   * CLASSE PLANEJADOR     *
   ************************* */

/// Torna o mapa vazio
/// NAO DEVE SER MODIFICADA
void Planejador::clear()
{
  pontos.clear();
  rotas.clear();
}

/// Funcao auxiliar para eliminar eventuais separadores do final de uma string.
/// NAO DEVE SER MODIFICADA
void trim(string& S)
{
  while (!S.empty() && isspace(S.back())) S.pop_back();
}

/// Leh um mapa dos arquivos arq_pontos e arq_rotas.
/// Caso nao consiga ler dos arquivos, deixa o mapa inalterado e
/// gera excecao ios_base::failure.
/// Deve receber ACRESCIMOS
void Planejador::ler(const std::string& arq_pontos,
                     const std::string& arq_rotas)
{
  // Vetores temporarios para armazenamento dos Pontos e Rotas lidos.
  /* ***********  /
  /  FALTA FAZER  /
  /  *********** */
  vector<Ponto> provPontos;
  vector<Rota> provRotas;

  vector<Ponto>::iterator itPonto;
  vector<Rota>::iterator itRota;

  string cabecalho;

  // Leh os Pontos do arquivo e armazena no vetor temporario de Pontos.
  // Em caso de qualquer erro, gera excecao ios_base::failure com mensagem:
  //   "Erro <CODIGO> na leitura do arquivo de pontos <ARQ_PONTOS>"
  try
  {
    // 1) Abre uma stream associada ao arquivo de Pontos
    //    (Em caso de erro, codigo 1)
    ifstream stream_inPontos(arq_pontos);
    if (!stream_inPontos.is_open()) throw 1;
    // 2) Consome eventuais separadores, leh o cabecalho do arquivo, elimina eventuais
    //    separadores no final da string e testa o cabecalho:
    //    "ID;Nome;Latitude;Longitude"
    //    (Em caso de erro ou valor lido diferente, codigo 2)
    stream_inPontos >> cabecalho; // consome enventuais delimitadores
    trim(cabecalho);//    Consome os separadores apos o cabecalho

    if (stream_inPontos.fail() || cabecalho != "ID;Nome;Latitude;Longitude") throw 2;

    // 3) Enquanto o arquivo nao acabar (eof), repita a leitura de cada um dos Pontos:
    while (!stream_inPontos.eof()) {
    	Ponto p;
    	string valor;
    	char c;

    //    | 3.1) Leh a ID e elimina eventuais separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 3)
    //    |      O teste se a ID eh valida serah feito ao testar o Ponto
    	getline(stream_inPontos, valor, ';');
    	trim(valor);

    	if (!stream_inPontos || valor == "" ) throw 3;
    	p.id.set(move(valor));
    //    | 3.2) Consome os separadores, leh o nome e elimina eventuais separadores no final
    //    |      da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 4)
    //    |      O teste se o nome eh valido serah feito ao testar o Ponto

		getline(stream_inPontos, valor, ';');
		trim(valor);

		if (!stream_inPontos || valor == "") throw 4;
		p.nome = move(valor);
    //    | 3.3) Leh a latitude
    //    |      (Em caso de erro, codigo 5)
    //    |      O teste se a latitude eh valida serah feito ao testar o Ponto
    	getline(stream_inPontos, valor, ";");
    	trim(valor);

    	if (!stream_inPontos || valor == "") throw 5;
    	p.latitude = move(valor);
    //    | 3.4) Leh o caractere ';'
    //    |      (Em caso de erro ou valor lido diferente, codigo 6)
    	c = stream_inPontos.get();
    	if (!stream_inPontos || c != ';') throw 6;

    //    | 3.5) Leh a longitude
    //    |      (Em caso de erro, codigo 7)
    //    |      O teste se a longitude eh valida serah feito ao testar o Ponto
    	getline(stream_inPontos, p.longitude);
    	if (!stream_inPontos || p.longitude == "") throw 7;
    //    | 3.6) Consome os separadores apos o Ponto
    	stream_inPontos >> ws;
    //    | 3.7) Testa se o Ponto com os parametros lidos eh valido
    //    |      (Em caso de erro, codigo 8)
    	if (!p.valid()) throw 8;
    //    | 3.8) Testa que nao existe Ponto com a mesma ID no vetor temporario
    //    |      de Pontos lidos ateh agora
    //    |      (Em caso de erro, codigo 9)
    	itPonto = find_if(provPontos.begin(), provPontos.end(), [p.id](Ponto ponto) {p.id == ponto.id;});
    	if (itPonto != provPontos.end()) throw 9;
    //    | 3.9) Insere o Ponto lido no vetor temporario de Pontos
    	provPontos.push_back(p);
	}
    // 4) Se nao foi lido nenhum Ponto, gera erro (codigo 10)
    if (provPontos.size() == 0) throw 10;

    // 5) Fecha o arquivo de Pontos
    stream_inPontos.close();
    /* ***********  /
    /  FALTA FAZER  /
    /  *********** */
  }
  catch (int i)
  {
    // Chama o destrutor de todas as variaveis criadas dentro do try, inclusive da
    // stream associada ao arquivo. Portanto, nao precisa fechar a stream.
    string msg = "Erro " + to_string(i) + " na leitura do arquivo de pontos " + arq_pontos;
    throw ios_base::failure(msg);
  }

  // Leh as Rotas do arquivo e armazena no vetor temporario de Rotas.
  // Em caso de qualquer erro, gera excecao ios_base::failure com mensagem:
  //   "Erro <CODIGO> na leitura do arquivo de rotas <ARQ_ROTAS>"
  try
  {
    // 1) Abre uma stream associada ao arquivo de Rotas
    //    (Em caso de erro, codigo 1)
    const string arquivoRotas("rotas.txt");
    ifstream stream_inRotas(arquivoRotas);

    if (!stream_inRotas.is_open()) throw 1;

    // 2) Consome eventuais separadores, leh o cabecalho do arquivo, elimina eventuais
    //    separadores no final da string e testa o cabecalho:
    //    "ID;Nome;Extremidade 1;Extremidade 2;Comprimento"
    //    (Em caso de erro ou valor lido diferente, codigo 2)
    //    Consome os separadores apos o cabecalho
    stream_inRotas >> cabecalho;
    trim(cabecalho);

    if (stream_inRotas.fail() || cabecalho != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento") throw 2;
    // 3) Enquanto o arquivo nao acabar (eof), repita a leitura de cada uma das Rotas:
    while (!stream_inRotas.eof()) {
    	Rota r;
	//    | 3.1) Leh a ID e elimina eventuais separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 3)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota
    	getline(stream_inRotas, r.id, ';');
    	trim(r.id);

    	if (!stream_inRotas || r.id == "") throw 3
    //    | 3.2) Consome os separadores, leh o nome e elimina eventuais separadores no final
    //    |      da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 4)
    //    |      O teste se o nome eh valido serah feito ao testar a Rota
    	getline(stream_inRotas, r.nome, ';');
    	trim(r.nome);

    	if (!stream_inRotas || r.nome == "") throw 4;
    //    | 3.3) Consome os separadores, leh a ID da extremidade[0] e elimina eventuais
    //    |      separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 5)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota
    	getline(stream_inRotas, r.extremidade[0], ';');
    	trim(r.extremidade[0]);

    	if (!stream_inRotas || r.extremidade[0] == "") throw 5;
    //    | 3.4) Consome os separadores, leh a ID da extremidade[1] e elimina eventuais
    //    |      separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 6)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota
    	getline(stream_inRotas, r.extremidade[1], ';');
    	trim(r.extremidade[1]);

    	if (!stream_inRotas || r.extremidade[1] == "") throw 6;
    //    | 3.5) Leh o comprimento
    //    |      (Em caso de erro na leitura, codigo 7)
    //    |      O teste se o comprimento eh valido serah feito ao testar o Ponto
    	getline(stream_inRotas, r.comprimento, ';');
    	trim(r.comprimento);

    	if (!stream_inRotas || r.comprimento == "") throw 7;
    //    | 3.6) Consome os separadores apos a Rota
    	stream_inRotas >> ws;
    //    | 3.7) Testa se a Rota com esses parametros lidos eh valida
    //    |      (Em caso de erro, codigo 8)
    	if (!r.valid()) throw 8;
    //    | 3.8) Testa que a Id da extremidade[0] corresponde a um ponto lido
    //    |      no vetor temporario de Pontos
    //    |      (Em caso de erro, codigo 9)
    	itRota = find_if(provPontos.begin(), provPontos.end(), [r](Rota rota){ r.extremidade[0] == rota.extremidade[0];});
    	if (itRota == provPontos.end()) throw 9;
    //    | 3.9) Testa que a Id da extremidade[1] corresponde a um ponto lido
    //    |      no vetor temporario de Pontos
    //    |      (Em caso de erro, codigo 10)
    	itRota = find_if(provPontos.begin(), provPontos.end(), [r](Rota rota){r.extremidade[1] == rota.extremidade[1]});
    	if (itRota == provPontos.end()) throw 10;
    //    | 3.10)Testa que nao existe Rota com a mesma ID no vetor temporario
    //    |      de Rotas lidas ateh agora
    //    |      (Em caso de erro, codigo 11)
    	itRota = find_if(provRotas.begin(), provRotas.end(), (r)[Rota rota]{rota.id == r.id;});
    	if (itRota != provRotas.end()) throw 11;
    //    | 3.11)Insere a Rota lida no vetor temporario de Rotas
    	provRotas.push_back(r);
    }
    // 4) Se nao foi lido nenhuma Rota, gera erro (codigo 12)
    if (provRotas.size() == 0) throw 12;
    // 5) Fecha o arquivo de Rotas
    stream_inRotas.close();
    /* ***********  /
    /  FALTA FAZER  /
    /  *********** */
  }
  catch (int i)
  {
    // Chama o destrutor de todas as variaveis criadas dentro do try, inclusive da
    // stream associada ao arquivo. Portanto, nao precisa fechar a stream.
    string msg = "Erro " + to_string(i) + " na leitura do arquivo de rotas " + arq_rotas;
    throw ios_base::failure(msg);
  }

  // Faz os vetores de Pontos e Rotas do planejador assumirem o conteudo dos
  // vetores temporarios de Pontos e Rotas
  rotas = move(provRotas);
  pontos = move(provPontos);
  /* ***********  /
  /  FALTA FAZER  /
  /  *********** */
}

