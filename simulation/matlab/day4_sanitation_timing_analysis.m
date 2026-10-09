%% Day 4 sanitation timing analysis
clear; clc; close all;
names=["Waste evacuation","Primary flush","Drain 1","Rinse 1","Drain 2","Final rinse","Final drain","Cycle verify","Complete hold"];
duration_s=[3.0 4.0 2.5 4.0 2.5 3.0 3.0 1.5 1.5];
start_s=[0 cumsum(duration_s(1:end-1))];finish_s=cumsum(duration_s);
T=table(names',start_s',finish_s',duration_s','VariableNames',{'State','Start_s','Finish_s','Duration_s'});disp(T);fprintf('Total programmed sanitation-cycle duration: %.1f s\n',sum(duration_s));
figure('Name','Day 4 - Sanitation State Timeline');hold on;for i=1:numel(names),plot([start_s(i) finish_s(i)],[i i],'LineWidth',8);end;yticks(1:numel(names));yticklabels(names);xlabel('Time (s)');ylabel('Sanitation state');title('Programmed Sanitation Sequence Timeline');grid on;xlim([0 sum(duration_s)+1]);writetable(T,'day4_sanitation_timing.csv');