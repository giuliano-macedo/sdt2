<h1>Problema dos Jantar dos Filósofos em Sistemas Distribuidos</h1>
esse projeto foi o segundo trabalho da disciplina de Sistemas Distribuidos UFMS-CPPP
<h2>funcionamento</h2>
<p>
Há dois tipos de nós da rede, o servidor (Mesa) e o cliente (Filósofo), o filósofo
só pode em estar em dois estados, comendo ou pensando, o pensar pode ser
o calculo de n digitos de pi, uma espera de n ms ou até o apertar do enter do usuário
quando ele termina de pensar ele requisita a mesa uma matriz não-singular quadrada,
e depois envia devolta sua matriz inversa, esse é o estado de comer, caso retorne uma matriz inválida
ele é expulso da mesa. 
</p>
<p>
Enquanto isso na mesa, ele organiza todos os filósofos numa pilha duplamente lincada
circular, de forma que cada nó tenha dois ponteiros para um mutex, um esquerdo e um direito, assim cada filósofo adjacente 
tem um mutex (garfo) em comum, cada filósofo só pode comer quando fizer o lock nos dois mutexes,
desta forma o algorítmo resolve o problema tratando de impasses e fome através de uma fila de prioridades, quando um filósofo não consegue
fazer o lock de ambos os garfos, ele se inclui numa fila, e depois essa fila será executada pelo nó que está ocupado esse garfo.
em resumo o algorítmo pode ser expresso nisso toda vez que uma requisição de comer for feita:
</p>
<code>
1.checar se a fila tem os garfos desse nó atual
	1.1se sim
		adicione-se na fila
		pare
	1.2 se não
		continue  
2.tenta fazer lock em dois garfos, se não conseguir um deles
	2.1 
		faça unlock de um deles e adicione-se na fila
		pare
	2.1 caso contrário
		mande matriz não singular quadrada para filósofo
		execute monitor
</code>
<p>
O monitor tem o propósito de atender o primeiro nó da fila e o remove da fila, desta forma ele garante o lock de ambos os garfos, e
impasses são tratados pelo fato do nó soltar um garfo se não conseguir ambos, e resolve fome por executar sempre o primeiro da fila.
</p>
<h2>Pré-requerimentos</h2>
<ul>
	<li>g++</li>
	<li>make</li>
	<li>OpenBlas</li>
	<li>LAPPACK</li>
	<li>armadillo</li>
	<li>POSIX sockets</li>
	<li>gmp</li>
</ul>
<h2>Executar</h2>
para executar a mesa basta usar o comando
<code>./table [caminho para arquivo mtx] [tempo máxio de pensamento de filósofo (ms)]</code>
o arquivo mtx pode ser gerado pelo binário genmtx com
<code>./genmtx [dimensão das matrizes] [número de matrizes] [saida.mtx]</code>
ou por outra fonte, o formato mtx é muito simples sendo constituído de seus 4 primeiros bytes
a dimensão da matriz e o resto a matriz em sí em ordem de coluna maior com cada elemento sendo 1 byte

para executar o filósof basta usar o comando
<code>./philosopher [ip para mesa] [modo] [parâmetros] ...</code>
sendo os modos
<code>
pi [menor digito] [digito máximo] 
sleep [min(ms)] [max(ms)] 
manual
</code>
<h2>compilar</h2>
para compilar todos os binários basta executar <code>make</code> caso algum binári específico
execute <code>make [binario específico]</code>