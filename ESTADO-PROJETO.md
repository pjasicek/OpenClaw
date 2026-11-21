# Estado do projeto — OpenClaw (fork nboliv/OpenClaw)

Atualizado em 21/11/2026. Objetivo: completar o jogo usando os dados originais
do CD/DVD físico (CLAW.REZ + MOVIES + MUSIC, copiados para
`~/Games/OpenClaw/Build_Release/` e linkados simbolicamente em `Build_Release/`).

## Como buildar e rodar

```sh
cd ~/vibe-coding-projects/OpenClaw
cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
      -DCMAKE_EXE_LINKER_FLAGS="-L/opt/homebrew/lib"
cmake --build build -j 8        # binário sai em Build_Release/openclaw
cd Build_Release && ./openclaw
```

- ffmpeg é dependência em runtime (via Homebrew, já instalado).
- `MOVIES/`, `MUSIC/`, `CUSTOM/`, `CLAW.REZ` são symlinks para a cópia do DVD.
- Permissões da cópia já corrigidas (`chmod -R u+w ~/Games/OpenClaw/Build_Release/`),
  o rsync tinha preservado as permissões read-only do UDF.

## Feito: cutscenes do DVD (branch `cutscenes-video`, commit a1d0eeb)

Playback real dos clipes MPEG-2 de `MOVIES/*.VOB`:

- **`OpenClaw/Engine/Video/ScreenElementVideo.{h,cpp}`** — decodifica via pipe do
  ffmpeg (`rawvideo rgb24`) para uma SDL texture, 720x480, ~30fps por acumulador
  de tempo. Letterbox 4:3 (os clipes são 4:3 apesar da resolução de armazenamento).
  Qualquer tecla/clique pula o clipe. Comando `video <arquivo>` toca qualquer clipe.
- **Áudio**: SDL2_mixer não decodifica AC3; a faixa é extraída **uma vez** com
  ffmpeg para `<nome>.VOB.wav` no diretório de trabalho do jogo e tocada via
  `Mix_PlayMusic`. WAV ausente = vídeo mudo (não bloqueia).
- **Integração `HumanView`**: tabela `kLevelVideos` mapeia fase → clipe. Fase 1 em
  new game mostra `CLAW0.VOB` (cinematic de abertura, 6:55) antes de carregar a
  fase; o load é re-enfileirado quando o vídeo termina, com guarda para não repetir.
- **Comando de console `loadlevel <1-14>`** — pula direto para uma fase (mesma
  semância do avanço de fase existente, `isNewGame=false`).

### Verificado em teste real (21/11)

- Vídeo renderiza no window do jogo com letterbox e áudio (log "Playing video",
  WAV extraído 5.6MB p/ CLAW2).
- Fim do vídeo → load pendente dispara → tela de loading da fase renderiza → fase carrega.
- Render NÃO congela (suspeita da sessão anterior era artefato de screenshot
  durante a tela de loading).

### Pegadinha conhecida

`ExecuteStartupCommands` roda no fim de **todo** load de fase
(`BaseGameLogic.cpp:421`). Portanto **não** colocar `loadlevel` (nem nenhum
comando que enfileire load) em `startup_commands.txt` — vira loop infinito de
load. No console interativo é seguro.

## Próximo passo imediato: mapear os clipes restantes

Os 9 clipes do DVD (duração medida):

| Clipe | Duração | Fase? |
|---|---|---|
| CLAW0.VOB | 6:55 | Abertura (fase 1, new game) — mapeado |
| CLAW1.VOB | 1:03 | ? |
| CLAW2.VOB | 0:33 | ? |
| CLAW3.VOB | 1:50 | ? |
| CLAW4.VOB | 0:32 | ? |
| CLAW5.VOB | 2:26 | ? |
| CLAW6.VOB | 0:39 | ? |
| CLAW7.VOB | 1:29 | ? |
| CLAW8.VOB | 2:16 | ? |

Assistir cada um (`video MOVIES/CLAWn.VOB`) e adicionar as linhas em `kLevelVideos`
(`HumanView.cpp`). Clipes intermediários são de fim de fase; o final (CLAW8?)
deve ser o encerramento. Definir se tocam no fim da fase (antes do ScoreScreen)
ou na entrada da fase seguinte.

## Restante do jogo (estado medido no código, não no README)

- **Fase 14 + chefe final** — não existe (issue #155 do autor upstream).
- **Fase 13 / chefe Red Tail** — jogável com chefe *dummy* (`e44c2ad`).
- **Fase 12 / Aquatis** — chefe simplificado (só melee parado).
- **Fase 10 / Marrow** — muito simplificado.
- **Fase 4 / Katherine** — não aparar tiros do Claw (issue #111).
- **Pegar e arremessar barris/inimigos** — não implementado (#174), mas já existe
  PR aberto #223 — não refazer, acompanhar.
- **Cheats do original** (#166) — parciais no console (ver startup_commands).
- **Música no macOS** — morta por design (`MidiProc.exe`/RPC do Windows); tocar
  os MIDIs via timidity/FluidSynth é trabalho separado.
- `SoundTriggerComponent`, `GlobalAmbientSoundComponent` — `TODO: Implement`.
- Vários `TODO: HACK` de física (queda, escada, plataformas) — dívida técnica.

Chefes bem-feitos: `GabrielEncounter.cpp` (559 linhas) é a referência de
qualidade; os outros chefes estão em 130–300 linhas.
