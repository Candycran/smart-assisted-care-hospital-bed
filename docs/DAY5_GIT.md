# Day 5 Git workflow

After adding caregiver reset and verifying it:

```powershell
git add .
git commit -m "feat: add safe caregiver fault and emergency reset"
git push
```

After final validation:

```powershell
git add .
git commit -m "test: complete end-to-end system verification"
git push
```

After risk/traceability documentation:

```powershell
git add .
git commit -m "docs: add requirements traceability and preliminary FMEA"
git push
```

After screenshots, GIFs and final README:

```powershell
git add .
git commit -m "docs: finalise smart assisted-care bed portfolio repository"
git push
```

Optional release tag:

```powershell
git tag -a v1.0-simulation -m "Complete virtual engineering prototype"
git push origin v1.0-simulation
```
